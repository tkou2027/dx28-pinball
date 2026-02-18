#pragma once
#include "object/game_object.h"

class PointLight : public GameObject
{
public:
	void Initialize() override;
	void SetColor(const Vector3& color);
	void SetLightActive(bool active);
private:
	ComponentId m_comp_id_render{};
	ComponentId m_comp_id_light{};
};