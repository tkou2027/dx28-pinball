#include "sprite_desc.h"

Sprite::Sprite(const SpriteDesc& desc)
{
	m_active = true;
	SetDesc(desc);
}

void Sprite::Initialize(const SpriteDesc& desc)
{
	m_active = true;
	SetDesc(desc);
}

void Sprite::SetDesc(const SpriteDesc& desc)
{
	m_desc = desc;
	m_uv_animation_state.uv_scroll_size = desc.initial_uv_scroll_size;
	m_uv_animation_state.uv_scroll_offset = desc.initial_uv_scroll_offset;
}

void Sprite::Update()
{
	if (!m_active)
	{
		return;
	}
	m_uv_animation_state.Update(m_desc.uv_animation_desc);
}

bool Sprite::GetOutput(SpriteOutput& sprite_output)
{
	if (!m_active)
	{
		return false;
	}
	// texture and color
	sprite_output.texture_id = m_desc.texture_id;
	sprite_output.color = m_desc.color;
	sprite_output.blend_add = m_desc.blend_add;
	// transform
	sprite_output.size = m_desc.size;
	sprite_output.offset = m_desc.offset;
	sprite_output.transform = m_transform.GetTransformGlobal();
	// uv
	const auto uv_rect = m_uv_animation_state.GetUVRect(m_desc.uv_animation_desc);
	sprite_output.uv_offset = uv_rect.uv_offset;
	sprite_output.uv_size = uv_rect.uv_size;
	// camera
	sprite_output.render_layer_mask = m_desc.render_layer_mask;

	return true;
}