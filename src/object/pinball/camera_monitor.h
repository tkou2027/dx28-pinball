#pragma once
#include "object/game_object.h"

class CameraMonitor : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
private:
	ComponentId m_comp_id_camera{};
};