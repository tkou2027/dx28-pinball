#pragma once
#include "object/game_object.h"

class PointLight : public GameObject
{
public:
	void Initialize() override;
	void SetColor(const Vector3& color);
	void Update() override;
	void SetLightActive(bool active);
	// editor
	Vector3& GetPosition() { return m_position; }
private:
	ComponentId m_comp_id_render{};
	ComponentId m_comp_id_light{};
	Vector3 m_position{ 1.0f, 1.0f, 1.0f };
};