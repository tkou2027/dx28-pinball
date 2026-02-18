#pragma once
#include "math/vector2.h"

enum class GoalType
{
	DEFAULT,
	// counter
	MAX
};

struct GoalPreset
{
	// ?
};

struct GoalConfig
{
	GoalType type{ GoalType::DEFAULT };
	int speed_level{ 3 };
	float radius{ 4.0f };
	float height{ 8.0f };
	Vector2 position{};
};

extern const GoalType g_goal_presets[static_cast<size_t>(GoalType::MAX)];