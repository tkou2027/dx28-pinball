#pragma once
#include "object/game_object.h"
#include "game_util/floor_config.h"

class RoomFloorVisual : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(const FloorConfig& config);
	void SetVisible(bool visible);
private:
	void InitializeFloorFace();
	void InitializeFloorBorder();
	void InitializeFloorSide();
	ComponentId m_comp_id_mesh{};
	FloorConfig m_config{};
};