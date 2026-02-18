#pragma once
#include "math/vector2.h"

enum class BumperType
{
	TRI_BUMPER,
	QUAD_BUMPER,
	// count
	MAX
};

struct BumperPreset
{
	int sides{};
	float radius{};
	float height{};
};

struct BumperConfig
{
	BumperType type{};
	float rotation{};
	Vector2 position{};
};

extern const BumperPreset g_bumper_presets[static_cast<size_t>(BumperType::MAX)];