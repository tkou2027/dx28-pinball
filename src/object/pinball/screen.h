#pragma once
#include "object/game_object.h"

#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "config/preset_manager.h"
#include "config/camera_names.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"

class Screen : public GameObject
{
public:
	void Initialize() override
	{
		for (int k = 0; k < 4; k++)
		{
			auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
			const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");

			auto& texture_loader = GetTextureLoader();
			const auto screen_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.screen_main.name);
			// int screen_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/test_screen_bg.png");
			const auto pixel_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png");

			MaterialDesc material_desc{};
			TechniqueDescDefault material_default{};
			material_default.emission_intensity = 2.0f;
			material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
			material_default.cull_type = CullType::CULL_NONE;
			material_default.shading_model = ShadingModel::UNLIT;
			material_default.albedo_texture_id = screen_texture_id;
			material_default.emission_texture_id = screen_texture_id;
			material_default.no_deferred = true;
			material_desc.SetTechnique(material_default, CameraRenderLayerMask::REFLECTED | CameraRenderLayerMask::DEFAULT);

			TechniqueDescForwardScreen material_screen{};
			material_screen.screen_pixel_texture_id = pixel_texture_id;
			material_screen.screen_pixels_scale = Vector2{ 1.0f, 1.0f };
			material_desc.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT);

			TechniqueDescForwardUnlit material_unlit{};
			material_desc.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

			Model model{ model_desc, material_desc, &m_transform };
			float size = 32.0f;
			model.GetTransform().SetScale({ size, size * 4.0f, size });
			// model.GetTransform().SetPosition({ 0.0f, 0.0f, -36.0f });
			// model.GetTransform().SetScale({ 1280, 720, size });
			float radius = 30.0f;

			{
				// distribute evenly around circle
				const float angle = static_cast<float>(k) * (Math::TWO_PI / 4.0f);
				const float px = sinf(angle) * radius;
				const float pz = cosf(angle) * radius;
				const float py = size * -1.0f; // adjust if you want vertical offset

				// position relative to parent (m_transform)
				model.GetTransform().SetPosition({ px, py, pz });

				// face the center: forward vector = center - position = (-px, 0, -pz)
				// yaw = atan2(forward.x, forward.z)
				const float yaw = atan2f(-px, -pz);
				model.GetTransform().SetRotationEuler({ 0.0f, yaw, 0.0f });
			}

			comp_render_mesh.AddModel(model);
		}

		//{
		//	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
		//	// const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");
		//	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_4x1");


		//	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
		//	// const auto texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_names.screen_camera);
		//	const auto& texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/screen_side.png");
		//	const auto pixel_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png");

		//	MaterialDesc material_desc{};
		//	TechniqueDescDefault material_default{};
		//	material_default.emission_intensity = 2.0f;
		//	material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
		//	material_default.cull_type = CullType::CULL_BACK;
		//	material_default.shading_model = ShadingModel::UNLIT;
		//	material_default.albedo_texture_id = texture_id;
		//	material_default.emission_texture_id = texture_id;
		//	material_default.no_deferred = true;
		//	material_default.cull_type = CullType::CULL_NONE;
		//	material_desc.SetTechnique(material_default, CameraRenderLayerMask::REFLECTED | CameraRenderLayerMask::DEFAULT);

		//	TechniqueDescForwardScreen material_screen{};
		//	material_screen.screen_pixel_texture_id = pixel_texture_id;
		//	material_desc.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT);

		//	TechniqueDescForwardUnlit material_unlit{};
		//	material_desc.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

		//	Model model{ model_desc, material_desc, &m_transform };
		//	float size = 26.0f;
		//	model.GetTransform().SetScale({ size, 10.0f,  size });
		//	model.GetTransform().SetPosition({ 0.0f, 18.0f, 0.0f });
		//	// model.GetTransform().SetScale({ 1280, 720, size });


		//	comp_render_mesh.AddModel(model);
		//}

		{
			auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
			// const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");
			const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_4x1");


			auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
			// const auto& texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_names.screen_camera);
			const auto& texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/screen_side.png");
			const auto pixel_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png");

			MaterialDesc material_desc{};
			TechniqueDescDefault material_default{};
			material_default.emission_intensity = 2.0f;
			material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
			material_default.cull_type = CullType::CULL_BACK;
			material_default.shading_model = ShadingModel::UNLIT;
			material_default.albedo_texture_id = texture_id;
			material_default.emission_texture_id = texture_id;
			material_default.no_deferred = true;
			material_default.cull_type = CullType::CULL_NONE;
			material_desc.SetTechnique(material_default, CameraRenderLayerMask::REFLECTED | CameraRenderLayerMask::DEFAULT);
			
			{
				float size = 45.0f;
				float height = 10.0f;
				float uv_scale = size * 1.414f * 4.0f / height;

				MaterialDesc material_desc_my{ material_desc };

				TechniqueDescForwardScreen material_screen{};
				material_screen.screen_pixel_texture_id = pixel_texture_id;
				material_screen.screen_pixels_scale = Vector2{ 4.0f * size * 1.414f / uv_scale / 64.0f, height / 64.0f };
				material_desc_my.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT);

				TechniqueDescForwardUnlit material_unlit{};
				material_desc_my.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

				Model model{ model_desc, material_desc_my, &m_transform };
				model.GetUVAnimationState().uv_scroll_size = Vector2{ uv_scale, 1.0f };
				model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);

				model.GetTransform().SetScale({ size, height,  size });
				model.GetTransform().SetPosition({ 0.0f, 24.0f, 0.0f });

				comp_render_mesh.AddModel(model);
			}
			{
				float size = 30.0f;
				float height = 8.0f;
				float uv_scale = size * 1.414f * 4.0f / height;

				MaterialDesc material_desc_my{ material_desc };

				TechniqueDescForwardScreen material_screen{};
				material_screen.screen_pixel_texture_id = pixel_texture_id;
				material_screen.screen_pixels_scale = Vector2{ 4.0f * size * 1.414f / uv_scale / 64.0f, height / 64.0f };
				material_desc_my.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT);

				//TechniqueDescForwardUnlit material_unlit{};
				//material_desc_my.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

				Model model{ model_desc, material_desc_my, &m_transform };
				model.GetUVAnimationState().uv_scroll_size = Vector2{ -uv_scale, 1.0f };

				model.GetTransform().SetScale({ size, height,  size });
				model.GetTransform().SetPosition({ 0.0f, 18.0f, 0.0f });

				comp_render_mesh.AddModel(model);
			}
			{
				float size = 28.0f;
				float height = 8.0f * size / 30.0f;
				float uv_scale = size * 1.414f * 4.0f / height;

				MaterialDesc material_desc_my{ material_desc };

				TechniqueDescForwardScreen material_screen{};
				material_screen.screen_pixel_texture_id = pixel_texture_id;
				material_screen.screen_pixels_scale = Vector2{ 4.0f * size * 1.414f / uv_scale / 64.0f, height / 64.0f };
				material_desc_my.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT);

				//TechniqueDescForwardUnlit material_unlit{};
				//material_desc_my.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

				Model model{ model_desc, material_desc_my, &m_transform };
				model.GetUVAnimationState().uv_scroll_size = Vector2{ uv_scale, 1.0f };

				model.GetTransform().SetScale({ size, height,  size });
				model.GetTransform().SetPosition({ 0.0f, 12.0f, 0.0f });

				comp_render_mesh.AddModel(model);
			}
		}
	}
private:
	ComponentId m_comp_id_mesh{};
	bool m_crushed{ false };
};