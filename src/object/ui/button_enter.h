#pragma once
#include "object/game_object.h"
#include "util/yoyo.h"

class ButtonEnter : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void EnterPressed();
private:
	enum class ButtonEnterState
	{
		IDLE,
		PRESSED
	};
	ComponentId m_comp_id_render{};
	void UpdateIdle();
	void UpdatePressed();
	Yoyo m_yoyo{ 0 };
	ButtonEnterState m_state{ ButtonEnterState::IDLE };
};