#include "fade.h"

#include "util/tween.h"
#include "component/render/component_renderer_sprite.h"
#include "config/preset_manager.h"
#include "config/palette.h"
#include "config/constant.h"


void Fade::Initialize()
{
	// center
	m_transform_2d.SetPosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });

	// sprites
	auto& comp_render = m_components.Add<ComponentRendererSprite>(m_comp_id_render);
	const auto& sprite_desc = GetPresetManager().GetSpriteDesc("sprite/ui/fade");
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		Sprite sprite{ sprite_desc };
		sprite.m_transform.SetParent(&m_transform_2d);
		sprite.m_uv_animation_state.uv_scroll_offset.x = i == 1 ? 0.5f : 0.0f;
		comp_render.AddSprite(sprite);
	}

	// update
	AddUpdateLayer(UpdateLayer::FADE);
	EnterIdle();
}

void Fade::InitializeConfig(const FadeConfig& config)
{
	m_config = config;
}

void Fade::Update()
{
	switch (m_state)
	{
	case FadeState::FADE_IN:
	{
		UpdateFadeIn();
		break;
	}
	case FadeState::FADE_OUT:
	{
		UpdateFadeOut();
		break;
	}
	}
}
void Fade::StartFadeIn(bool fade_in_out)
{
	m_countdown = m_config.duration;
	m_fade_in_out = fade_in_out;
	m_fade_out_in = false;
	m_state = FadeState::FADE_IN;

	auto& render_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_render);
	render_comp.GetSprite(0).m_uv_animation_state.uv_scroll_offset.y = 0;
	render_comp.GetSprite(1).m_uv_animation_state.uv_scroll_offset.y = 0;

	render_comp.SetActive(true);
	SetSceneUpdateLayer(UpdateLayer::FADE);
}

void Fade::StartFadeOut(bool fade_out_in)
{
	m_countdown = m_config.duration;
	m_fade_out_in = fade_out_in;
	m_fade_in_out = false;
	m_state = FadeState::FADE_OUT;

	auto& render_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_render);
	render_comp.GetSprite(0).m_uv_animation_state.uv_scroll_offset.y = -SCREEN_HEIGHT;
	render_comp.GetSprite(1).m_uv_animation_state.uv_scroll_offset.y = SCREEN_HEIGHT;

	render_comp.SetActive(true);
	SetSceneUpdateLayer(UpdateLayer::FADE);
}

bool Fade::IsIdle() const
{
	return m_state == FadeState::IDLE;
}

void Fade::EnterIdle()
{
	auto& render_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_render);
	render_comp.SetActive(false);
	m_state = FadeState::IDLE;
	m_fade_in_out = false;
	m_fade_out_in = false;
	SetSceneUpdateLayer(UpdateLayer::DEFAULT);
}

void Fade::UpdateFadeIn()
{
	if (m_countdown == 0)
	{
		if (m_fade_in_out)
		{
			m_fade_in_out = false;
			UpdateFadeOut();
		}
		else
		{
			EnterIdle();
		}
	}
	float t = 1.0f - static_cast<float>(m_countdown) / m_config.duration;
	Tween::EaseFunc(Tween::TweenFunction::EASE_OUT_QUAD, t);
	auto& render_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_render);
	render_comp.GetSprite(0).m_uv_animation_state.uv_scroll_offset.y = -SCREEN_HEIGHT * t;
	render_comp.GetSprite(1).m_uv_animation_state.uv_scroll_offset.y = SCREEN_HEIGHT * t;

	m_countdown--;
}

void Fade::UpdateFadeOut()
{
	if (m_countdown == 0)
	{
		if (m_fade_out_in)
		{
			m_fade_out_in = false;
			UpdateFadeIn();
		}
		else
		{
			EnterIdle();
		}
		return;
	}
	float t = 1.0f - static_cast<float>(m_countdown) / m_config.duration;
	t = Tween::EaseFunc(Tween::TweenFunction::EASE_IN_QUAD, t);
	auto& render_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_render);
	auto& sprites = render_comp.GetSprites();
	sprites[0].m_uv_animation_state.uv_scroll_offset.y = -SCREEN_HEIGHT * (1.0f - t);
	sprites[1].m_uv_animation_state.uv_scroll_offset.y = SCREEN_HEIGHT * (1.0f - t);

	m_countdown--;
}