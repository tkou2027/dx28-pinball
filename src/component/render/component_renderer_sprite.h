#pragma once
#include <vector>
#include "component/component.h"
#include "render/config/sprite_desc.h"

class ComponentRendererSprite : public Component
{
public:
	ComponentRendererSprite(int sprite_num = 1);
	void Update() override;
	int AddSprite(const Sprite& state);
	std::vector<Sprite>& GetSprites() { return m_sprite_states; }
	Sprite& GetSprite(int index) { return m_sprite_states[index]; }
	std::vector<SpriteOutput> GetRenderData();
private:
	std::vector<Sprite> m_sprite_states;
	std::vector<SpriteOutput> m_sprite_outputs;
};