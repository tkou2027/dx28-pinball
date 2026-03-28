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
	auto& comp_particle = m_components.Add<ComponentRendererParticle>(m_comp_id_particle);
	comp_particle.SetActive(false);
}

void ScreenSquare::InitializeConfig(
	const TextureResourceId& screen_texture_id, const Vector2& screen_pixels_scale,
	float width, float height, int index, int sides)
{
	m_crushed = false;

	m_width = width;
	m_height = height;
	m_index = index;
	m_sides = sides;

	const Vector2 uv_scroll_size = Vector2{ 1.0f / static_cast<float>(sides), 1.0f };
	const Vector2 uv_scroll_offset = Vector2{ uv_scroll_size.x * (sides - index - 1.0f), 0.0f };

	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");

	auto& texture_loader = GetTextureLoader();
	// const auto screen_texture_id = screen_texture_id;// texture_loader.GetOrCreateRenderTextureId(g_camera_presets.screen_main.name);
	const auto pixel_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png"); // TODO: linear
	const auto world_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.monitor.name);

	{
		// screen mesh
		MaterialDesc material_desc{};
		TechniqueDescDefault material_default{};
		material_default.emission_intensity = 2.0f;
		material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
		material_default.cull_type = CullType::CULL_NONE;
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.albedo_texture_id = screen_texture_id;
		material_default.emission_texture_id = screen_texture_id;
		material_default.no_deferred = true;
		material_desc.SetTechnique(material_default, CameraRenderLayerMask::REFLECTED | CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::REFLECTED_PLANE);

		TechniqueDescForwardScreen material_screen{};
		material_screen.screen_pixel_texture_id = pixel_texture_id;
		material_screen.world_texture_id = world_texture_id;
		material_screen.screen_pixels_scale = screen_pixels_scale;
		material_desc.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::REFLECTED_PLANE);

		TechniqueDescForwardUnlit material_unlit{};
		material_desc.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

		Model model{ model_desc, material_desc, &m_transform };
		model.GetTransform().SetScale({ m_width, m_height, 1.0f });
		model.GetUVAnimationState().uv_scroll_size = uv_scroll_size;
		model.GetUVAnimationState().uv_scroll_offset = uv_scroll_offset;

		m_id_mesh_screen = comp_render_mesh.AddModel(model);
	}

	{
		// screen mesh background
		MaterialDesc material_desc{};
		TechniqueDescDefault material_default{};
		material_default.cull_type = CullType::CULL_NONE;
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.base_color = Vector3{ 0.01f, 0.01f, 0.01f };
		material_default.emission_intensity = 0.0f;
		material_default.emission_color = Vector3{ 0.0f, 0.0f, 0.0f };
		material_default.no_deferred = true;
		material_desc.SetTechnique(material_default, CameraRenderLayerMask::REFLECTED | CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::REFLECTED_PLANE);

		TechniqueDescForwardScreen material_screen{};
		material_screen.screen_pixel_texture_id = pixel_texture_id;
		material_screen.world_texture_id = world_texture_id;
		material_screen.screen_pixels_scale = screen_pixels_scale;
		material_desc.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::REFLECTED_PLANE);

		TechniqueDescForwardUnlit material_unlit{};
		material_desc.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

		Model model{ model_desc, material_desc, &m_transform };
		model.GetTransform().SetScale({ m_width, m_height, 1.0f });
		model.GetTransform().SetPosition({ 0.0f, 0.0f, 0.1f });
		model.GetUVAnimationState().uv_scroll_size = uv_scroll_size;
		model.GetUVAnimationState().uv_scroll_offset = uv_scroll_offset;

		model.SetActive(false);
		m_id_mesh_background = comp_render_mesh.AddModel(model);
	}

	{
		// particle emitter
		auto& emitter_comp = m_components.Get<ComponentRendererParticle>(m_comp_id_particle);
		EmitterDesc conf{};
		conf.shape = TextureParticleShape::PLANE;
		conf.num_width = screen_pixels_scale.x * 2;
		conf.num_height = screen_pixels_scale.y * 2;
		conf.local_mesh_size = Vector2{ m_width / static_cast<float>(conf.num_width), m_height / static_cast<float>(conf.num_height) };
		conf.texture_id = screen_texture_id;
		conf.local_texture_id = pixel_texture_id;
		conf.lifetime = 5.0f;
		conf.gravity = Vector3{ 0.0f, -9.8f * 2.0f, 0.0f };

		TransformNode3D transform{};
		transform.SetParent(&m_transform);
		transform.SetScale({ m_width, m_height, 1.0f });
		// transform.SetPosition();
		emitter_comp.InitializeEmitter(conf, transform);

		emitter_comp.GetUVAnimationState().uv_scroll_size = uv_scroll_size;
		emitter_comp.GetUVAnimationState().uv_scroll_offset = uv_scroll_offset;
		emitter_comp.SetActive(false);


	}
}

void ScreenSquare::Crush()
{
	if (m_crushed)
	{
		return;
	}
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model_screen = comp_render_mesh.GetModel(m_id_mesh_screen);
	model_screen.SetActive(false);
	auto& model_background = comp_render_mesh.GetModel(m_id_mesh_background);
	model_background.SetActive(true);

	auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_particle);
	comp_render_particle.SetActive(true);
	comp_render_particle.InitializeBurst();
	m_crushed = true;
}
