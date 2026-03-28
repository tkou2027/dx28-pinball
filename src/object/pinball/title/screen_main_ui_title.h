#pragma once
#include "object/game_object.h"
#include "util/yoyo_timer.h"

class ScreenMainUITitle : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void Crush();
private:
	void UpdateDelay();
	void InitializeSprites();
	ComponentId m_comp_id_sprite{};
	ComponentId m_comp_id_mesh{};
	struct UIConfig
	{
		float max_radius{ 1.0f };
		float min_radius{ 0.0f };
		float max_thickness{ 0.5f };
		float min_thickness{ 0.04f };
		float animation_interval{ 2.0f };
	};
	UIConfig m_config{};
	// states
	YoyoTimer m_animation_timer{};
};