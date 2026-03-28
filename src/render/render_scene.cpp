
#include "render_scene.h"

#include <chrono>
#include <algorithm>
#include "global_context.h"
#include "render_system.h"
#include "render/render_resource.h"
#include "scene/scene_manager.h"
#include "component/render/component_renderer_sprite.h"
#include "component/render/component_renderer_mesh.h"
#include "component/render/component_renderer_particle.h"
#include "component/render/component_renderer_billboard.h"
#include "component/render/component_light.h"
#include "component/render/component_camera.h"
#include "render/config/camera_data.h"
#include "util/debug_ostream.h"
#include "config/preset_manager.h"

using namespace DirectX;

void RenderScene::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_billboards.Initialize(device, context);
}

void RenderScene::Finalize()
{
}

void RenderScene::Update()
{
	UpdateCameras();
	UpdateObjects();
	UpdateLights();
}

void RenderScene::UpdateRelease()
{
	auto& render_swap_context = g_global_context.m_render_system->GetSwapContext();
	const auto& camera_swap_data = render_swap_context.GetSwapData().camera_data;
	m_camera_manager.UpdateRelease(camera_swap_data);
	render_swap_context.ResetCameraSwapData();
}

void RenderScene::UpdateCameras()
{
	auto& render_swap_context = g_global_context.m_render_system->GetSwapContext();
	const auto& camera_swap_data = render_swap_context.GetSwapData().camera_data;
	m_camera_manager.Update(camera_swap_data);
	render_swap_context.ResetCameraSwapData();
}

void RenderScene::UpdateLights()
{
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	if (!scene)
	{
		return;
	}
	auto& component_manager = scene->GetObjectList().GetComponentManager();
	m_lights.num_point_lights = 0;

	const auto& lights = component_manager.GetComponents<ComponentLight>();
	for (const auto& light_comp : lights)
	{
		if (!light_comp.GetActive())
		{
			continue;
		}
		auto& light = m_lights.point_lights[m_lights.num_point_lights];
		light.color = light_comp.GetColor().ToXMFLOAT3();
		light.position_w = light_comp.GetPosition().ToXMFLOAT3();
		light.attenuation_radius = light_comp.GetAttenuationRadius();

		m_lights.num_point_lights++;
	}

	m_light_models.clear();
	m_light_models.reserve(m_lights.num_point_lights);
	const auto& model_desc = g_global_context.m_preset_manager->GetModelDesc("geo/unit_cube"); // "model/primitive/iso_sphere

	for (int i = 0; i < m_lights.num_point_lights; i++)
	{
		const auto& light = m_lights.point_lights[i];
		ModelRenderInfo light_model{};

		light_model.key.model_type = ModelType::STATIC;
		light_model.key.model_id = model_desc.model_id;
		// light_model.key.material_id = material_id;

		const float scale = light.attenuation_radius * 2.0f;
		XMMATRIX matScale = XMMatrixScaling(scale, scale, scale);
		XMMATRIX matTrans = XMMatrixTranslation(light.position_w.x, light.position_w.y, light.position_w.z);
		XMMATRIX matModel = XMMatrixMultiply(matScale, matTrans);
		XMStoreFloat4x4(&light_model.instance.model_matrix, matModel);

		m_light_models.push_back(light_model);
	}
}

void RenderScene::UpdateObjects()
{
	auto start = std::chrono::high_resolution_clock::now();
	m_renderables.Update();
	auto finish = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "updating objects" << duration.count() << " " << std::endl;
}

void RenderScene::ProcessModelData(std::vector<Model>& models)
{
	auto& render_resource = g_global_context.m_render_system->GetRenderResource();
	for (auto& model : models)
	{
		if (!model.GetActive())
		{
			continue;
		}
		render_resource.GetModelLoader().GetOrLoadModel(model.GetModelDesc()); // TODO: models not loaded on start
		ModelRenderInfo info{};
		model.GetRenderKey(info.key);
		model.GetRenderInstance(info.instance);
		if (model.GetModelDesc().has_animation)
		{
			m_objects_main_skinned.push_back(info);
		}
		else
		{
			m_objects_main.push_back(info);
		}
	}
}

// Renderables
void SceneRenderablesManager::Update()
{
	Reset();
	UpdateSceneData();
	UpdateViewData();
}

bool SceneRenderablesManager::GetModelsOfCamera(RenderCameraBase* camera, std::vector<size_t>& out_indices_model) const
{
	assert(camera);
	const auto& camera_config = camera->GetUsageConfig();
	const auto& render_layer = camera_config.render_layer;
	const auto& layer_renderables = m_view_renderables[static_cast<size_t>(render_layer)];
	if (!camera_config.enable_frustum_culling)
	{
		out_indices_model = layer_renderables.indices_model;
		return false;
	}
	AABB camera_bbox = camera->GetFrustumBoundingBox();
	for (size_t model_index : layer_renderables.indices_model)
	{
		const auto& model = m_models[model_index];
		if (model.key.bbox_id >= 0)
		{
			const auto& bbox_data = m_bounding_boxes[model.key.bbox_id];
			if (!camera_bbox.Overlaps(bbox_data.bbox))
			{
				continue;
			}
			out_indices_model.push_back(model_index);
		}
		else
		{
			out_indices_model.push_back(model_index);
		}
	}
	return true;
}

void SceneRenderablesManager::Reset()
{
	m_models.clear();
	m_sprites.clear();
	m_particles.clear();
	m_bounding_boxes.clear();
	m_models.reserve(30000);
	for (auto& view_renderables : m_view_renderables)
	{
		view_renderables.indices_model.clear();
		view_renderables.indices_model.reserve(30000);
		view_renderables.indices_sprite.clear();
	}
}

void SceneRenderablesManager::UpdateSceneData()
{
	// TODO: partial update
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	if (!scene)
	{
		return;
	}
	auto& component_manager = scene->GetObjectList().GetComponentManager();
	// meshes
	auto& mesh_comps = component_manager.GetComponents<ComponentRendererMesh>();
	for (auto& mesh_comp : mesh_comps)
	{
		if (!mesh_comp.GetActive())
		{
			continue;
		}
		auto& models = mesh_comp.GetModels();
		ProcessModelData(models);
	}
	std::sort(
		m_models.begin(), m_models.end(),
		[](const ModelRenderInfo& a, const ModelRenderInfo& b)
		{
			if (a.key.material_id != b.key.material_id)
			{
				return a.key.material_id < b.key.material_id;
			}
			if (a.key.model_type < b.key.model_type)
			{
				return a.key.model_type < b.key.model_type;
			}
			// if (a.key.)
			return a.key.model_id < b.key.model_id;
		}
	);


	// sprites
	auto& sprites = component_manager.GetComponents<ComponentRendererSprite>();
	for (auto& sprite_comp : sprites)
	{
		if (!sprite_comp.GetActive())
		{
			continue;
		}
		auto sprites = sprite_comp.GetRenderData();
		for (auto& sprite : sprites)
		{
			m_sprites.push_back(sprite);
		}
	}

	// billboards
	// {
	// 	m_billboards.ClearData();
	// 	auto& comps = component_manager.GetComponents<ComponentRendererBillboard>();
	// 	for (const auto& comp : comps)
	// 	{
	// 		m_billboards.AddInstances(comp.GetDesc(), comp.GetInstances());
	// 	}
	// 	m_billboards.UpdateData();
	// }
}

void SceneRenderablesManager::UpdateViewData()
{
	auto& material_resource = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	for (int model_index = 0; model_index < m_models.size(); model_index++)
	{
		const auto& model_info = m_models[model_index];
		const auto& material_desc = material_resource.GetMaterialDesc(model_info.key.material_id);
		for (int layer_index = 0; layer_index < static_cast<int>(CameraRenderLayer::MAX); layer_index++)
		{
			CameraRenderLayer layer = static_cast<CameraRenderLayer>(layer_index);
			if (material_desc.IfRenderLayer(layer))
			{
				m_view_renderables[layer_index].indices_model.push_back(model_index);
			}
		}
	}

	// sprites
	for (int sprite_index = 0; sprite_index < m_sprites.size(); sprite_index++)
	{
		const auto& layer_mask = m_sprites[sprite_index].render_layer_mask;
		for (int layer_index = 0; layer_index < static_cast<int>(CameraRenderLayer::MAX); layer_index++)
		{
			CameraRenderLayer layer = static_cast<CameraRenderLayer>(layer_index);
			if (CameraRenderLayerMask::IfLayerInMask(layer_mask, layer))
			{
				m_view_renderables[layer_index].indices_sprite.push_back(sprite_index);
			}
		}
	}
}

void SceneRenderablesManager::ProcessModelData(std::vector<Model>& models)
{
	auto& render_resource = g_global_context.m_render_system->GetRenderResource();
	const auto& camera_manager = g_global_context.m_render_system->GetRenderScene().GetCameraManager();
	for (auto& model : models)
	{
		if (!model.GetActive())
		{
			continue;
		}
		render_resource.GetModelLoader().GetOrLoadModel(model.GetModelDesc()); // TODO: models not loaded on start
		ModelRenderInfo info{};
		model.GetRenderKey(info.key);
		model.GetRenderInstance(info.instance);
		if (!model.GetBoundingBox().Empty())
		{
			ModelBoundingBoxData bbox_data{};
			bbox_data.bbox = model.GetBoundingBox();
			info.key.bbox_id = m_bounding_boxes.size();
			m_bounding_boxes.push_back(bbox_data);
		}
		m_models.push_back(info);
	}
}
