#pragma once
#include "math/vector2.h"

struct UVFrameAnimationDesc
{
	// uv transform
	Vector2 uv_size{ 1.0f, 1.0f };
	Vector2 uv_offset{ 0.0f, 0.0f };
	Vector2 uv_size_per_frame{ 1.0f, 1.0f }; // to be computed
	// uv animation
	int frame_cols{ 1 };
	int frame_rows{ 1 };
	int frame_total{ 1 };
	int frame_offset{ 0 };
	int frame_default{ 0 };
	int play_speed_scale{ 1 };
	bool play_loop{ false };
	bool uv_wrap{ false };
};