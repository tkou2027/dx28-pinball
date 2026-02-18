#pragma once
#include "object/game_object.h"

class ScreenSquare : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(float width, float height, int index, int sides);
	void Update() override;
	void Crush();
private:
	ComponentId m_comp_id_mesh{};
	ComponentId m_comp_id_particle{};

	float m_width{};
	float m_height{};
	int m_index{ -1 };
	int m_sides{};
	bool m_crushed{ false };
};