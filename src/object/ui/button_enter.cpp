#include "button_enter.h"

#include "util/tween.h"
#include "component/render/component_renderer_sprite.h"
#include "config/preset_manager.h"
#include "config/palette.h"

void ButtonEnter::Initialize()
{
	// sprite
	auto& comp_render = m_components.Add<ComponentRendererSprite>(m_comp_id_render);
	const auto& sprite_desc = GetPresetManager().GetSpriteDesc("sprite/ui/button_enter");
	Sprite sprite{ sprite_desc };
	sprite.m_transform.SetParent(&m_transform_2d);
	comp_render.AddSprite(sprite);

	// animation
	m_yoyo.Initialize(60);
}

void ButtonEnter::Update()
{
	switch (m_state)
	{
	case ButtonEnterState::IDLE:
	{
		UpdateIdle();
		break;
	}
	case ButtonEnterState::PRESSED:
	{
		UpdatePressed();
		break;
	}
	}
}

void ButtonEnter::EnterPressed()
{
	m_yoyo.Initialize(5);
	m_state = ButtonEnterState::PRESSED;
}

void ButtonEnter::UpdateIdle()
{
	m_yoyo.Update();
	float t = Tween::EaseFunc(Tween::TweenFunction::EASE_IN_OUT_QUAD, m_yoyo.GetT());
	auto& comp_render = m_components.Get<ComponentRendererSprite>(m_comp_id_render);
	auto& sprite = comp_render.GetSprite(0);
	sprite.m_desc.offset.y = t * 4.0f;
}

void ButtonEnter::UpdatePressed()
{
	m_yoyo.Update();
	float t = Tween::EaseFunc(Tween::TweenFunction::EASE_IN_OUT_QUAD, m_yoyo.GetT());
	auto& comp_render = m_components.Get<ComponentRendererSprite>(m_comp_id_render);
	auto& sprite = comp_render.GetSprite(0);
	sprite.m_desc.color.w = t;
}
