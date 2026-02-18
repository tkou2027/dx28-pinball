#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"

class TitleManager : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
private:
	enum class TitleState
	{
		IDLE,
		EASE_OUT
	};
	struct TitleConfig
	{
		float ease_out_delay{ 0.5f };
	};
	void UpdateIdle();
	void EnterEaseOut();
	void UpdateEaseOut();
	TitleState m_state{ TitleState::IDLE };
	TitleConfig m_config{};
	CountdownTimer m_state_countdown{};
	std::weak_ptr<class ButtonEnter> m_button_enter{};
};