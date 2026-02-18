#pragma once
#include "object/game_object.h"
#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "render/render_resource.h"
#include "config/preset_manager.h"

class TestTexture : public GameObject
{
public:
	void Initialize() override
	{
		{
			auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
			// const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");
			const auto& model_desc = GetPresetManager().GetModelDesc("model/tv_face");
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
		//	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
		//	// const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");
		//	const auto& model_desc = GetPresetManager().GetModelDesc("model/tv_box");
		//	//const auto& model_desc = GetPresetManager().GetModelDesc("model/primitive/iso_sphere");

		//	MaterialDesc material_desc{};
		//	TechniqueDescDefault material_default{};
		//	material_default.emission_intensity = 2.0f;
		//	material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
		//	material_default.cull_type = CullType::CULL_BACK;
		//	material_default.no_deferred = true;
		//	material_desc.SetTechnique(material_default);

		//	TechniqueDescForwardGlass material_glass{};
		//	auto& texture_loader = GetTextureLoader();
		//	material_glass.env_texture_id = texture_loader.GetOrCreateRenderTextureId("reflect");
		//	material_desc.SetTechnique(material_glass);

		//	auto& material_resource = GetMaterialResource();
		//	int material_id = material_resource.AddMaterialDescIfNotExists("mat_test_glass2", material_desc);

		//	material_desc.SetTechnique(TechniqueDescForwardUnlit{});
		//	Model model{ model_desc, material_desc, &m_transform };
		//	float size = 10.0f;
		//	model.GetTransform().SetScale(size);

		//	comp_render_mesh.AddModel(model);
		//}
	}
private:
	ComponentId m_comp_id_mesh{};
};