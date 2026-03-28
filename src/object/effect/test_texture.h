#pragma once
#include "object/game_object.h"
#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "render/render_resource.h"
#include "config/preset_manager.h"

#include "object/game_object_list.h"
#include "object/pinball/camera_follow.h"
#include "component/render/component_camera.h"

class TestTexture : public GameObject
{
public:
	void Initialize() override
	{
		auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
		{
			// const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");
			const auto& model_desc = GetPresetManager().GetModelDesc("model/tv");
			//const auto& model_desc = GetPresetManager().GetModelDesc("model/primitive/iso_sphere");

			MaterialDesc material_desc{};
			TechniqueDescDefault material_default{};
			material_default.emission_intensity = 2.0f;
			material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
			material_default.cull_type = CullType::CULL_BACK;
			material_default.no_deferred = true;
			material_desc.SetTechnique(material_default);

			TechniqueDescForwardGlass material_glass{};
			auto& texture_loader = GetTextureLoader();
			material_glass.env_texture_id = texture_loader.GetOrCreateRenderTextureId("reflect");
			material_desc.SetTechnique(material_glass);

			auto& material_resource = GetMaterialResource();
			int material_id = material_resource.AddMaterialDescIfNotExists("mat_test_glass", material_desc);

			Model model{ model_desc, material_id, &m_transform };
			float size = 10.0f;
			model.GetTransform().SetScale(size);

			comp_render_mesh.AddModel(model);
		}
		//{
		//	// const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");
		//	const auto& model_desc = GetPresetManager().GetModelDesc("model/tv_box");
		//	//const auto& model_desc = GetPresetManager().GetModelDesc("model/primitive/iso_sphere");

		//	MaterialDesc material_desc{};
		//	TechniqueDescDefault material_default{};
		//	material_default.base_color = Vector3{ 1.0f, 1.0f, 1.0f } * 0.0f;
		//	material_default.cull_type = CullType::CULL_BACK;
		//	material_default.shading_model = ShadingModel::UNLIT;
		//	material_desc.SetTechnique(material_default);

		//	auto& material_resource = GetMaterialResource();
		//	int material_id = material_resource.AddMaterialDescIfNotExists("mat_test_box", material_desc);

		//	Model model{ model_desc, material_id, &m_transform };
		//	float size = 10.0f;
		//	model.GetTransform().SetScale(size);

		//	comp_render_mesh.AddModel(model);
		//}
	}
	void OnSceneInitialized() override
	{
		auto camera_main = GetOwner().FindGameObject<CameraFollow>();
		m_comp_id_reference_camera_main =
			camera_main->GetComponents().TryGetComponent<ComponentCamera>();
		m_camera_main = camera_main;
		assert(m_camera_main.lock());
	}
	void Update() override
	{
		const auto& comp_camera_main = GetOwner().GetComponentManager().GetComponent<ComponentCamera>(m_comp_id_reference_camera_main);
		Vector3 main_forward = comp_camera_main.GetTarget() - comp_camera_main.GetPosition();
		main_forward.GetNormalized();

		auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
		auto& models = comp_render_mesh.GetModels();
		AABB bbox{ Vector3{ 1.0f, 1.0f, 1.0f } *-1.0f, Vector3{ 1.0f, 1.0f, 1.0f } *1.0f };
		bbox = bbox + GetTransform().GetPosition();
		models[0].SetBoundingBox(bbox);
		//models[1].SetBoundingBox(bbox);

		const Vector3 my_forward = GetTransform().GetForwardGlobal();
		float dot = Vector3::Dot(main_forward, my_forward);
		bool active = dot < 0.0f;
		comp_render_mesh.SetActive(active);
	}
private:
	ComponentId m_comp_id_mesh{};
	ComponentId m_comp_id_reference_camera_main{};
	std::weak_ptr<class GameObject> m_camera_main{};
};