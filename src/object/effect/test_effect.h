#pragma once
#include "object/game_object.h"
#include "component/render/component_renderer_particle.h"
#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "config/preset_manager.h"
#include "global_context.h"
#include "platform/keyboard.h"

class TestEffect : public GameObject
{
public:
	void Initialize() override
	{
		// m_transform.SetRotationYOnly(Math::PI);
		{
			auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
			const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");

			auto& texture_loader = GetTextureLoader();
			const auto screen_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/bg_texture_color.png");
			// int screen_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/test_screen_bg.png");
			const auto pixel_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png");

			MaterialDesc material_desc{};
			TechniqueDescDefault material_default{};
			material_default.emission_intensity = 2.0f;
			material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
			material_default.cull_type = CullType::CULL_BACK;
			material_default.shading_model = ShadingModel::UNLIT;
			material_default.albedo_texture_id = screen_texture_id;
			material_default.emission_texture_id = screen_texture_id;
			material_default.no_deferred = true;
			material_desc.SetTechnique(material_default);

			TechniqueDescForwardScreen material_screen{};
			material_screen.screen_pixel_texture_id = pixel_texture_id;
			material_desc.SetTechnique(material_screen);

			Model model{ model_desc, material_desc, &m_transform };
			float size = 10.0f;
			model.GetTransform().SetScale({ size, size * 4.0f, size });
			// model.GetTransform().SetScale({ 1280, 720, size });


			comp_render_mesh.AddModel(model);
		}

		{
			auto& comp = m_components.Add<ComponentRendererParticle>(m_comp_id_particle);
			auto& particle_item = comp.GetTextureParticleItem();
			TextureParticleConfig conf{
			   TextureParticleShape::PLANE,
			   64, 256
			};
			auto& texture_loader = GetTextureLoader();
			conf.texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/bg_texture_color.png");
			conf.local_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png");

			TransformNode3D transform{};
			transform.SetParent(&m_transform);

			float size = 10.0f;
			transform.SetPositionY(-size * 2.0f);
			transform.SetScale({ size, size * 4.0f, size });
			particle_item.SetConfig(
				conf,
				UVFrameAnimationDesc{},
				transform
			);
			m_crushed = false;
			particle_item.InitializeParticle();

			comp.SetActive(false);
		}


	}

	void Update() override
	{
		auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_particle);
		auto& particle_item = comp_render_particle.GetTextureParticleItem();
		particle_item.Update();

		auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
		
		if (g_global_context.m_keyboard->IsKeyTrigger(KK_SPACE))
		{
			if (!m_crushed)
			{
				comp_render_mesh.SetActive(false);
				comp_render_particle.SetActive(true);
				particle_item.EnterUpdateCrush();
				m_crushed = true;
			}
			else
			{
				particle_item.ResetParticle();
				comp_render_mesh.SetActive(true);
				comp_render_particle.SetActive(false);
				m_crushed = false;
			}
		}
	}
private:
	ComponentId m_comp_id_mesh{};
	ComponentId m_comp_id_particle{};
	bool m_crushed{ false };
};