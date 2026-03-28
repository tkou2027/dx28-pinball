#pragma once
#include "object/game_object.h"
#include "render/config/texture_resource_id.h"

class ScreenSquare : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(
		const TextureResourceId& screen_texuture_id,
		const Vector2& pixels_scale,
		float width, float height, int index, int sides);
	void Crush();
private:
	ComponentId m_comp_id_mesh{};
	ComponentId m_comp_id_particle{};
	int m_id_mesh_screen{ -1 };
	int m_id_mesh_background{ -1 };

	float m_width{};
	float m_height{};
	int m_index{ -1 };
	int m_sides{};
	bool m_crushed{ false };
};