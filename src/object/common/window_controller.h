#pragma once
#include "object/game_object.h"

class WindowController : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
private:
	bool m_full_screen{ false };
};