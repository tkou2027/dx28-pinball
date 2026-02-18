#include "screen_square.h"
#include "component/render/component_renderer_mesh.h"
#include "component/render/component_renderer_particle.h"
#include "render/render_resource.h"
#include "render/resource/texture_loader.h"
#include "config/preset_manager.h"
#include "config/camera_names.h"

void ScreenSquare::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
	m_components.Add<ComponentRendererParticle>(m_comp_id_particle);
}

void ScreenSquare::InitializeConfig(float width, float height, int index, int sides)
{
	m_width = width;
	m_height = height;
	m_index = index;
	m_sides = sides;

	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");

	auto& texture_loader = GetTextureLoader();
	const auto screen_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.screen_main.name);
	const auto pixel_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png"); // TODO: linear
	const auto world_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.monitor.name);

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
	material_screen.world_texture_id = world_texture_id;
	material_screen.screen_pixels_scale = Vector2{
		(float)g_camera_presets.screen_main.width, (float)g_camera_presets.screen_main.height } * 0.15f; // TODO
	material_desc.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT);

	TechniqueDescForwardUnlit material_unlit{};
	material_desc.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

	Model model{ model_desc, material_desc, &m_transform };
	model.GetTransform().SetScale({ m_width, m_height, 1.0f });
	float u_scale = 1.0f / static_cast<float>(sides);
	model.GetUVAnimationState().uv_scroll_size = Vector2{ u_scale, 1.0f };
	model.GetUVAnimationState().uv_scroll_offset = Vector2{ u_scale * m_index, 1.0f };

	comp_render_mesh.AddModel(model);

	{
		auto& comp = m_components.Get<ComponentRendererParticle>(m_comp_id_particle);
		auto& particle_item = comp.GetTextureParticleItem();
		TextureParticleConfig conf{
		   TextureParticleShape::PLANE,
		   m_width, m_height // TODO: config
		};
		auto& texture_loader = GetTextureLoader();
		conf.texture_id = screen_texture_id;
		conf.local_texture_id = pixel_texture_id;

		TransformNode3D transform{};
		transform.SetParent(&m_transform);
		transform.SetScale({ m_width, m_height, 1.0f });
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

void ScreenSquare::Update()
{
	if (!m_crushed)
	{
		return;
	}
	auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_particle);
	auto& particle_item = comp_render_particle.GetTextureParticleItem();
	particle_item.Update();
}

void ScreenSquare::Crush()
{
	if (m_crushed)
	{
		return;
	}
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	comp_render_mesh.SetActive(false);

	auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_particle);
	auto& particle_item = comp_render_particle.GetTextureParticleItem();
	comp_render_particle.SetActive(true);
	particle_item.EnterUpdateCrush();
	m_crushed = true;
}
