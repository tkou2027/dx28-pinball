#pragma once
#include "object/game_object.h"

class FloorTitle : public GameObject
{
public:
	void Initialize() override;
private:
	ComponentId m_comp_id_mesh{};
};