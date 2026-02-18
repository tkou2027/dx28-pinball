#include "component_renderer_sprite.h"

#include "global_context.h"
#include "render/render_system.h"
#include "object/game_object.h"

ComponentRendererSprite::ComponentRendererSprite(int num_sprites)
{
	m_sprite_states.reserve(num_sprites);
}

void ComponentRendererSprite::Update()
{
	for (auto& sprite_state : m_sprite_states)
	{
		sprite_state.Update();
	}
}

int ComponentRendererSprite::AddSprite(const Sprite& state)
{
	int index = static_cast<int>(m_sprite_states.size());
	m_sprite_states.push_back(state);
	return index;
}

std::vector<SpriteOutput> ComponentRendererSprite::GetRenderData()
{
	std::vector<SpriteOutput> outputs;
	for (auto& sprite_state : m_sprite_states)
	{
		if (!sprite_state.m_active)
		{
			continue;
		}
		SpriteOutput output{};
		sprite_state.GetOutput(output);
		outputs.push_back(output);
	}
	return outputs;
}
