#pragma once
#include <vector>
#include "math/vector2.h"
#include "math/vector3.h"
#include "bumper_config.h"
#include "goal_config.h"
#include "floor_config.h"
#include "enemy_config.h"

struct RoomConfig
{
	// basic settings
	//float radius{};
	//float radius_inner{};
	//float height{};
	// actions
	Vector3 camera_center{};
	float trans_in_height_offset{ -40.0f };
	float trans_out_height_offset{ -40.0f };

	Vector3 player_trans_in_position{};
	Vector3 player_active_position{};

	FloorConfig floor{};
	std::vector<BumperConfig> bumpers{};
	std::vector<GoalConfig> goals{};
	std::vector<EnemyCenterConfig> enemies_center{};
};

struct FieldConfig
{
	std::vector<RoomConfig> rooms{};
};


extern FieldConfig g_field_config;