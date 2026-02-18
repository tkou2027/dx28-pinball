
#include "render_scene.h"
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

#include <chrono>
#include "util/debug_ostream.h"
#include <algorithm>
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
	UpdateParticles();
}

void RenderScene::UpdateCameras()
{
	auto& render_swap_context = g_global_context.m_render_system->GetSwapContext();
	const auto& camera_swap_data = render_swap_context.GetSwapData().camera_data;
	m_camera_manager.Update(camera_swap_data);
	render_swap_context.ResetCameraSwapData();

	//for (auto& camera_config : camera_swap_data.cameras_to_add)
	//{
	//
	//}
	//
	//Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	//if (!scene)
	//{
	//	return;
	//}
	//
	//auto& component_manager = scene->GetObjectList().GetComponentManager();
	//{
	//	auto& camera_comps = component_manager.GetComponents<ComponentCamera>();
	//	bool has_main{ false };
	//	for (auto& camera_comp : camera_comps)
	//	{
	//		if (!camera_comp.GetActive())
	//		{
	//			continue;
	//		}
	//		const auto& usage_config = camera_comp.GetUsageConfig();
	//		int camera_id{};
	//		if (!camera_comp.IfInitialized())
	//		{
	//			m_camera_manager.CreateRenderCamera(
	//				usage_config, camera_comp.GetShapeConfig(), camera_id);
	//
	//			// TODO: init texture
	//			camera_comp.SetCameraId(camera_id);
	//		}
	//		auto& camera = m_camera_manager.GetCamera(camera_id);
	//		camera.SetTransform(
	//			camera_comp.GetPosition(),
	//			camera_comp.GetTarget(),
	//			camera_comp.GetUp()
	//		);
	//	}
	//	//if (!has_main)
	//	//{
	//	//	hal::dout << "RenderScene::UpdateCameras: main camera not found." << std::endl;
	//	//	assert(false);
	//	//}
	//}

	//m_views.clear(); // TODO: dirty flag

	// cameras from cameras ==============
	// auto& component_manager = scene->GetObjectList().GetComponentManager();
	// {
	// 	const auto& camera_comps = component_manager.GetComponents<ComponentCameraLegacy>();
	// 	bool has_main{ false };
	// 	for (auto& camera_comp : camera_comps)
	// 	{
	// 		const auto camera_data = camera_comp.GetCameraData();
	// 		//m_cameras.push_back(camera_data);
	// 		if (camera_data.config.type == CameraType::CAMERA_MAIN)
	// 		{
	// 			if (has_main)
	// 			{
	// 				hal::dout << "RenderScene::UpdateCameras: multiple main cameras found." << std::endl;
	// 			}
	// 			else
	// 			{
	// 				m_camera_data_main = camera_data;
	// 				has_main = true;
	// 			}
	// 		}
	// 	}
	// 	if (!has_main)
	// 	{
	// 		hal::dout << "RenderScene::UpdateCameras: main camera not found." << std::endl;
	// 		assert(false);
	// 	}
	// }

	// cameras from shadows ==============
	// TODO

	// cameras from reflection probes ==============
	//{
	//	const auto& camera_comps = component_manager.GetComponents<ComponentCameraCube>();
	//	for (auto& camera_comp : camera_comps)
	//	{
	//		const auto camera_data = camera_comp.GetCameraData();
	//		// TODO: check usage type
	//		m_reflection_cameras.push_back(camera_data);
	//	}
	//}

	// setup views =============
	// for (const auto& camera_data : m_cameras)
	// {
	// 	RenderScene::ViewContext view{};
	// 	view.camera_data = camera_data;
	// 	m_views.push_back(view);
	// }
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

		m_lights.num_point_lights++;
	}

	m_light_models.clear();
	m_light_models.reserve(m_lights.num_point_lights);
	const auto& model_desc = g_global_context.m_preset_manager->GetModelDesc("geo/unit_cube");

	// auto& material_manager = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	// MaterialDesc material_desc{};
	// material_desc.SetTechnique(TechniqueDescDefault{});
	// int material_id = material_manager.AddMaterialDescWithGeneratedKey(material_desc);

	for (int i = 0; i < m_lights.num_point_lights; i++)
	{
		const auto& light = m_lights.point_lights[i];
		ModelRenderInfo light_model{};

		light_model.key.model_type = ModelType::STATIC;
		light_model.key.model_id = model_desc.model_id;
		// light_model.key.material_id = material_id;

		const float scale = 30.0f;
		XMMATRIX matScale = XMMatrixScaling(scale, scale, scale);
		XMMATRIX matTrans = XMMatrixTranslation(light.position_w.x, light.position_w.y, light.position_w.z);
		XMMATRIX matModel = XMMatrixMultiply(matScale, matTrans);
		XMStoreFloat4x4(&light_model.instance.model_matrix, matModel);

		m_light_models.push_back(light_model);
	}
}

void RenderScene::UpdateObjects()
{
	// {
	// 	auto start = std::chrono::high_resolution_clock::now();
	// 
	// 	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	// 	if (!scene)
	// 	{
	// 		return;
	// 	}
	// 	auto& component_manager = scene->GetObjectList().GetComponentManager();
	// 
	// 	// sprites
	// 	m_sprites.clear();
	// 	auto& sprites = component_manager.GetComponents<ComponentRendererSprite>();
	// 	for (auto& sprite_comp : sprites)
	// 	{
	// 		if (!sprite_comp.GetActive())
	// 		{
	// 			continue;
	// 		}
	// 		auto sprites = sprite_comp.GetRenderData();
	// 		for (auto& sprite : sprites)
	// 		{
	// 			m_sprites.push_back(sprite);
	// 		}
	// 	}
	// 
	// 	// meshes // TODO const?
	// 	m_objects_main.clear();
	// 	m_objects_main_skinned.clear();
	// 	auto& mesh_comps = component_manager.GetComponents<ComponentRendererMesh>();
	// 	for (auto& mesh_comp : mesh_comps)
	// 	{
	// 		if (!mesh_comp.GetActive())
	// 		{
	// 			continue;
	// 		}
	// 		auto& models = mesh_comp.GetModels();
	// 		ProcessModelData(models);
	// 	}
	// 	// billboards
	// 	{
	// 		m_billboards.ClearData();
	// 		auto& comps = component_manager.GetComponents<ComponentRendererBillboard>();
	// 		for (const auto& comp : comps)
	// 		{
	// 			m_billboards.AddInstances(comp.GetDesc(), comp.GetInstances());
	// 		}
	// 		m_billboards.UpdateData();
	// 
	// 		auto finish = std::chrono::high_resolution_clock::now();
	// 		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	// 		hal::dout << "updating objects legacy" << duration.count() << " " << std::endl;
	// 	}
	// }

	auto start = std::chrono::high_resolution_clock::now();

	m_renderables.Update();

	auto finish = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "updating objects" << duration.count() << " " << std::endl;
}

void RenderScene::UpdateParticles()
{
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	if (!scene)
	{
		return;
	}
	auto& component_manager = scene->GetObjectList().GetComponentManager();
	m_particles.clear();
	auto& comps = component_manager.GetComponents<ComponentRendererParticle>();
	for (const auto& comp : comps)
	{
		if (!comp.GetActive())
		{
			continue;
		}
		m_particles.push_back(comp.GetTextureParticleItem());
	}
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

void SceneRenderablesManager::Reset()
{
	// m_models_static.clear();
	// m_models_skinned.clear();
	// m_models_instanced.clear();
	m_models.clear();
	m_sprites.clear();
	m_particles.clear();
	m_models.reserve(30000);
	for (auto& view_renderables : m_view_renderables)
	{
		// view_renderables.indices_static.clear();
		// view_renderables.indices_skinned.clear();
		// view_renderables.indices_instanced.clear();
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
	// ProcessModelDataToLayers(ModelType::STATIC);
	// ProcessModelDataToLayers(ModelType::SKINNED);
	// ProcessModelDataToLayers(ModelType::INSTANCED);

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
		m_models.push_back(info);
		// if (model.GetModelDesc().has_animation)
		// {
		// 	m_models_skinned.push_back(info);
		// }
		// else
		// {
		// 	m_models_static.push_back(info);
		// }
	}
}

// std::vector<ModelRenderInfo>& SceneRenderablesManager::GetModelsOfType(ModelType model_type)
// {
// 	switch (model_type)
// 	{
// 	case ModelType::STATIC:
// 	{
// 		return m_models_static;
// 	}
// 	case ModelType::SKINNED:
// 	{
// 		return m_models_skinned;
// 	}
// 	case ModelType::INSTANCED:
// 	{
// 		return m_models_instanced;
// 	}
// 	default:
// 	{
// 		assert(false);
// 		return m_models_static;
// 	}
// 	}
// }

// void SceneRenderablesManager::ProcessModelDataToLayers(ModelType model_type)
// {
// 	const std::vector<ModelRenderInfo>& models = GetModelsOfType(model_type);
// 	auto& material_resource = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
// 	for (int model_index = 0; model_index < models.size(); model_index++)
// 	{
// 		const auto& model_info = models[model_index];
// 		const auto& material_desc = material_resource.GetMaterialDesc(model_info.key.material_id);
// 		for (int layer_index = 0; layer_index < static_cast<int>(CameraRenderLayer::MAX); layer_index++)
// 		{
// 			CameraRenderLayer layer = static_cast<CameraRenderLayer>(layer_index);
// 			if (material_desc.IfRenderLayer(layer))
// 			{
// 				switch (model_type)
// 				{
// 				case ModelType::STATIC:
// 				{
// 					m_view_renderables[layer_index].indices_static.push_back(model_index);
// 					break;
// 				}
// 				case ModelType::SKINNED:
// 				{
// 					m_view_renderables[layer_index].indices_skinned.push_back(model_index);
// 					break;
// 				}
// 				case ModelType::INSTANCED:
// 				{
// 					m_view_renderables[layer_index].indices_instanced.push_back(model_index);
// 					break;
// 				}
// 				}
// 			}
// 		}
// 	}
// }
