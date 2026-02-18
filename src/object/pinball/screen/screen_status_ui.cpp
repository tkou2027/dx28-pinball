#include "screen_status_ui.h"
#include "component/render/component_renderer_sprite.h"
#include "config/camera_names.h"
#include "config/preset_manager.h"

void ScreenStatusUI::Initialize()
{
	m_components.Add<ComponentRendererSprite>(m_comp_id_sprite);
	InitializeSprites();
	SetSpritesSize();
}

void ScreenStatusUI::InitializeConfig(float value_full)
{
	m_value_full = value_full;
	m_value_curr = value_full;
}

void ScreenStatusUI::Update()
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
		UpdateBorderAnimation();
		break;
	}
	case State::DELAY:
	{
		UpdateDelay();
		break;
	}
	}
}

void ScreenStatusUI::UpdateValue(float value)
{
	value = Math::Clamp(value, 0.0f, m_value_full);
	if (value == m_value_curr)
	{
		return;
	}
	m_value_prev = m_value_curr;
	m_value_curr = value;
	m_state = State::DELAY;
	m_delay_timer.Initialize(m_config.delay_duration);

	// update value bar
	auto& sprite_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	auto& bar_value = sprite_comp.GetSprite(id_bar_value);
	float length = m_bar_length * (m_value_full > 0.0f ? m_value_curr / m_value_full : 1.0f);
	bar_value.m_transform.SetScaleX(length);
	bar_value.m_transform.SetPositionX(length * 0.5f);
}

void ScreenStatusUI::UpdateBorderAnimation()
{
	auto& sprite_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	// border rotation
	for (int i = 0; i < NUM_BORDERS; ++i)
	{
		auto& border = sprite_comp.GetSprite(id_borders[i]);
		border.m_uv_animation_state.uv_scroll_offset.x += m_config.border_rotation_speed * GetDeltaTime();
		if (border.m_uv_animation_state.uv_scroll_offset.x <= -1.0f)
		{
			border.m_uv_animation_state.uv_scroll_offset.x += 1.0f;
		}
		if (border.m_uv_animation_state.uv_scroll_offset.x >= 1.0f)
		{
			border.m_uv_animation_state.uv_scroll_offset.x -= 1.0f;
		}
	}

	//m_bar_offset_x += m_config.bar_rotation_speed * GetDeltaTime();
	//if (m_bar_offset_x <= 0.0f)
	//{
	//	m_bar_offset_x += m_bar_length;
	//}
	//if (m_bar_offset_x >= m_bar_length)
	//{
	//	m_bar_offset_x -= m_bar_length;
	//}
	//auto& bar_value = sprite_comp.GetSprite(id_bar_value);
	//bar_value.m_transform.SetPositionX(m_bar_offset_x);
	//auto& bar_back = sprite_comp.GetSprite(id_bar_back);
	//bar_back.m_transform.SetPositionX(m_bar_offset_x);
}

void ScreenStatusUI::UpdateDelay()
{
	float t = m_delay_timer.Update(GetDeltaTime());
	float value_delay = Math::Lerp(m_value_prev, m_value_curr, 1.0f - t);
	if (t <= 0.0f)
	{
		m_state = State::ACTIVE;
		value_delay = m_value_curr;
		// update sprites before return
	}
	// update back sprite
	auto& sprite_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	auto& bar_back = sprite_comp.GetSprite(id_bar_back);
	float length = m_bar_length * (m_value_full > 0.0f ? value_delay / m_value_full : 1.0f);
	bar_back.m_transform.SetScaleX(length);
	bar_back.m_transform.SetPositionX(length * 0.5f);
}

void ScreenStatusUI::InitializeSprites()
{
	auto& preset_manager = GetPresetManager();
	auto& sprite = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	// border
	{
		const auto& border_desc = preset_manager.GetSpriteDesc("sprite/status/border");
		for (int i = 0; i < NUM_BORDERS; ++i)
		{
			Sprite border_sprite{ border_desc };
			border_sprite.m_transform.SetParent(&m_transform_2d);
			id_borders[i] = sprite.AddSprite(border_sprite);
		}
	}
	// bar back (add before value)
	{
		const auto& bar_back_desc = preset_manager.GetSpriteDesc("sprite/status/bar_back");
		Sprite bar_back_sprite{ bar_back_desc };
		bar_back_sprite.m_transform.SetParent(&m_transform_2d);
		id_bar_back = sprite.AddSprite(bar_back_sprite);
	}
	// bar value
	{
		const auto& bar_value_desc = preset_manager.GetSpriteDesc("sprite/status/bar_value");
		Sprite bar_value_sprite{ bar_value_desc };
		bar_value_sprite.m_transform.SetParent(&m_transform_2d);
		id_bar_value = sprite.AddSprite(bar_value_sprite);
	}
}

void ScreenStatusUI::SetSpritesSize()
{
	auto& camera_preset = g_camera_presets.screen_status;
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

	// bars
	const float bar_y = screen_height * 0.5f;
	const float bar_height = roundf(Math::Max(screen_height * m_config.bar_height_ratio, 1.0f));
	auto& bar_back = sprite_comp.GetSprite(id_bar_back);
	bar_back.m_transform.SetScale({ screen_width, bar_height });
	bar_back.m_transform.SetPosition({ center_x, bar_y });
	bar_back.m_uv_animation_state.uv_scroll_size.x = screen_width / bar_height;

	auto& bar_value = sprite_comp.GetSprite(id_bar_value);
	bar_value.m_transform.SetScale({ screen_width, bar_height });
	bar_value.m_transform.SetPosition({ center_x, bar_y });
	bar_value.m_uv_animation_state.uv_scroll_size.x = screen_width / bar_height;
}
