#include "screen_main_ui.h"
#include "component/render/component_renderer_sprite.h"
#include "component/render/component_renderer_mesh.h"
#include "config/camera_names.h"
#include "config/preset_manager.h"
#include "config/palette.h"

void ScreenMainUI::Initialize()
{
	m_components.Add<ComponentRendererSprite>(m_comp_id_sprite);
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
	InitializeSprites();
	SetSpritesSize();
}

void ScreenMainUI::InitializeConfig(float value_full)
{
	m_value_full = value_full;
	m_value_curr = value_full;
}

void ScreenMainUI::Update()
{
	auto& sprite_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	// border rotation
	{
		for (int i = 0; i < NUM_BORDERS; ++i)
		{
			auto& border = sprite_comp.GetSprite(id_borders[i]);
			border.m_uv_animation_state.uv_scroll_offset.x += m_config.border_rotation_speed * GetDeltaTime();
		}
	}
	switch (m_state)
	{
	case State::IDLE:
	{
		break;
	}
	case State::ACTIVE:
	{
		UpdateAnimation();
		break;
	}
	case State::DELAY:
	{
		UpdateDelay();
		break;
	}
	}
}

void ScreenMainUI::UpdateAnimation()
{
}

void ScreenMainUI::UpdateDelay()
{

}

void ScreenMainUI::SetAimInfo(const AimInfo& aim_info)
{
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(0);
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.center_u = aim_info.rotation_ratio;
	mat.center_v = aim_info.height_ratio;
	mat.radius = aim_info.radius;
	mat.thickness = aim_info.thickness;
}

void ScreenMainUI::InitializeSprites()
{
	auto& preset_manager = GetPresetManager();
	auto& sprite = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	// border
	{
		const auto& border_desc = preset_manager.GetSpriteDesc("sprite/main/border");
		for (int i = 0; i < NUM_BORDERS; ++i)
		{
			Sprite border_sprite{ border_desc };
			border_sprite.m_transform.SetParent(&m_transform_2d);
			id_borders[i] = sprite.AddSprite(border_sprite);
		}
	}

	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane_xy");
		MaterialDesc material_desc{};
		TechniqueDescForwardScreenBackground material_screen_bg{};
		material_screen_bg.color_inside = g_palette.color_player;
		material_screen_bg.color_inside.w = 0.3f; // for mask
		material_screen_bg.color_stripe_positive = g_palette.color_theme;
		material_screen_bg.color_stripe_negative = g_palette.color_theme_accent;
		material_desc.SetTechnique(material_screen_bg, CameraRenderLayerMask::SCREEN_MAIN);
		Model model{ model_desc, material_desc, &m_transform };
		mesh_comp.AddModel(model);
	}
}

void ScreenMainUI::SetSpritesSize()
{
	auto& camera_preset = g_camera_presets.screen_main;
	const float screen_width = static_cast<float>(camera_preset.width);
	const float screen_height = static_cast<float>(camera_preset.height);
	const float center_x = screen_width * 0.5f;
	m_bar_length = screen_width;
	m_bar_offset_x = center_x;

	auto& sprite_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	// border
	const float border_height = Math::Max(screen_height * m_config.border_height_ratio, 1.0f);
	const float border_offset_y = border_height * 0.5f;
	for (int i = 0; i < NUM_BORDERS; ++i)
	{
		auto& border = sprite_comp.GetSprite(id_borders[i]);
		border.m_transform.SetScale({ screen_width, border_height });
		border.m_transform.SetPosition({ center_x, i == 0 ? border_offset_y : screen_height - border_offset_y });
		border.m_uv_animation_state.uv_scroll_size.x = screen_width / border_height;
	}

	//// bars
	// mesh
	const float center_y = screen_height * 0.5f;
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(0);
	model.GetTransform().SetScale({ screen_width, screen_height, 1.0f });
	model.GetTransform().SetPosition({ center_x, center_y, 0.5f }); // 0.5f to avoid clipping
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.radius = 0.6f;
	mat.center_u = 0.8f;
	mat.center_v = 0.05f;
}
