#include "screen_main_ui_title.h"
#include "component/render/component_renderer_sprite.h"
#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "config/preset_manager.h"
#include "config/palette.h"
#include "config/camera_names.h"

void ScreenMainUITitle::Initialize()
{
	m_components.Add<ComponentRendererSprite>(m_comp_id_sprite);
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
	m_animation_timer.Initialize(m_config.animation_interval);
	InitializeSprites();
}

void ScreenMainUITitle::Update()
{
	float t = m_animation_timer.Update(GetDeltaTime());
	// update animation
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(0);
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.radius = Math::Lerp(m_config.min_radius, m_config.max_radius, t);
	mat.thickness = Math::Lerp(m_config.max_thickness, m_config.min_thickness, t);
	mat.rotation = t * Math::PI;
}

void ScreenMainUITitle::Crush()
{
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(0);
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.color_stripe_positive = g_palette.color_player;
	mat.color_stripe_negative = g_palette.color_player;
}

void ScreenMainUITitle::InitializeSprites()
{
	auto& preset_manager = GetPresetManager();
	auto& texture_loader = GetTextureLoader();
	const auto& camera_preset = g_camera_presets.screen_main;
	const Vector2 center_uv{ 0.5f, 0.9f };
	auto& sprite = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	{
		const auto& title_text = preset_manager.GetSpriteDesc("sprite/ui/title_text");
		Sprite title_text_sprite{ title_text };
		title_text_sprite.m_desc.render_layer_mask = CameraRenderLayerMask::SCREEN_MAIN;
		title_text_sprite.m_transform.SetPosition({ camera_preset.width * center_uv.x, camera_preset.height - title_text_sprite.m_desc.size.y * 0.5f });

		sprite.AddSprite(title_text_sprite);
	}
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane_xy");
		MaterialDesc material_desc{};
		TechniqueDescForwardScreenBackground material_screen_bg{};
		material_screen_bg.color_inside = Vector4{};
		material_screen_bg.color_stripe_positive = g_palette.color_theme;
		material_screen_bg.color_stripe_negative = g_palette.color_theme_accent;
		material_screen_bg.center_u = center_uv.x;
		material_screen_bg.center_v = center_uv.y;
		material_desc.SetTechnique(material_screen_bg, CameraRenderLayerMask::SCREEN_MAIN);
		Model model{ model_desc, material_desc, &m_transform };
		model.GetTransform().SetScale(Vector3{ static_cast<float>(camera_preset.width), static_cast<float>(camera_preset.height), 1.0f });
		mesh_comp.AddModel(model);
	}
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane_xy");
		MaterialDesc material_desc{};
		TechniqueDescDefault material_default{};
		const auto& camera_preset_monitor = g_camera_presets.monitor;
		material_default.albedo_texture_id = texture_loader.GetOrCreateRenderTextureId(camera_preset_monitor.name);
		material_default.cull_type = CullType::CULL_NONE;
		material_default.no_deferred = true; // for unlit
		material_desc.SetTechnique(material_default, CameraRenderLayerMask::SCREEN_MAIN);
		material_desc.SetTechnique(TechniqueDescForwardUnlit{}, CameraRenderLayerMask::SCREEN_MAIN);
		Model model{ model_desc, material_desc, &m_transform };
		float model_width = static_cast<float>(camera_preset.width);
		float model_height = model_width * camera_preset_monitor.height / camera_preset_monitor.width;
		model.GetTransform().SetScale(Vector3{ model_width, model_height, 1.0f });
		// mesh projection matrix is different form sprite (no off center)
		// notice y is not flipped for mesh (different form sprite)
		model.GetTransform().SetPosition({
			camera_preset.width * (center_uv.x - 0.5f), camera_preset.height * (0.5f - center_uv.y), 0.5f });
		mesh_comp.AddModel(model);
	}
}
