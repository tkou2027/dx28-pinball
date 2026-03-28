#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"

class PlayerTitle : public GameObject
{
public:
	void Initialize() override;
private:
	ComponentId m_comp_id_mesh;
};