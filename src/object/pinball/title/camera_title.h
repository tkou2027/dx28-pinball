#pragma once

#include "object/game_object.h"

class CameraTitle : public GameObject
{
public:
	void Initialize() override;
private:
	ComponentId m_comp_id_camera{};
};