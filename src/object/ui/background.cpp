#include "background.h"
#include "util/tween.h"
#include "component/render/component_renderer_sprite.h"
#include "config/preset_manager.h"
#include "config/palette.h"
#include "config/constant.h"

void Background::Initialize()
{
	m_transform_2d.SetPosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	auto& comp_render = m_components.Add<ComponentRendererSprite>(m_comp_id_render);
}

void Background::SetSprite(const std::string& key)
{

	auto& comp_render = m_components.Add<ComponentRendererSprite>(m_comp_id_render);
	const auto& sprite_desc = GetPresetManager().GetSpriteDesc(key);
	Sprite sprite{ sprite_desc };
	sprite.m_transform.SetParent(&m_transform_2d);
	comp_render.AddSprite(sprite);
}
