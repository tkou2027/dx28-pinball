#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"

class ScreenStatusUI : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(float value_full);
	void Update() override;
	void UpdateValue(float value);
private:
	void UpdateBorderAnimation();
	void UpdateDelay();
	void InitializeSprites();
	void SetSpritesSize();
	ComponentId m_comp_id_sprite{};
	static constexpr int NUM_BORDERS{ 2 };
	int id_borders[NUM_BORDERS]{};
	int id_bar_value{};
	int id_bar_back{}; // delay

	// status
	enum class State
	{
		IDLE,
		ACTIVE,
		DELAY
	};
	State m_state{ State::ACTIVE };
	float m_value_full{};
	float m_value_prev{};
	float m_value_curr{};
	float m_bar_length{};
	float m_bar_offset_x{};
	CountdownTimer m_delay_timer{};

	struct ScreenStatusUIConfig
	{
		// layout
		float border_height_ratio{ 0.2f };
		float bar_height_ratio{ 0.33f };

		// action
		float value_full{ 1.0f };
		float delay_duration{ 2.0f };
		float border_rotation_speed{ -2.0f };
		float bar_rotation_speed{ 10.0f };
	};
	ScreenStatusUIConfig m_config{};
};