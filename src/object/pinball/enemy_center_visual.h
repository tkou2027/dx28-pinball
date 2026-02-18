#pragma once
#include "object/game_object.h"
#include "game_util/enemy_config.h"

class ScreenSquare;
class ScreenCylinder;
class EnemyCenterVisual : public GameObject
{
public:
	struct AimInfo
	{
		Vector3 position{};
	};
	void Initialize();
	void InitializeConfig(const EnemyCenterShapeConfig& shape_config);
	void SetAimInfo(const AimInfo& aim_info);
	static void AdjustShapeConfig(EnemyCenterShapeConfig& in_out_shape_config);
	void SetScreenCrushed(int index);
private:
	std::vector<std::weak_ptr<ScreenSquare> > m_screen_squares;
	std::weak_ptr<ScreenCylinder> m_screen_status;
	EnemyCenterShapeConfig m_config{};
	ComponentId m_comp_id_mesh{};
};