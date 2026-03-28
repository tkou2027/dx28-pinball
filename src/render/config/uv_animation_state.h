#pragma once
#include <vector>
#include "math/vector2.h"
#include "uv_animation_desc.h"

struct UVAnimationState
{
	int animation_index{ 0 };
	// frame animation
	bool playing{ false };
	int play_count{ 0 };
	int frame{ 0 };
	// scroll / scale
	Vector2 uv_scroll_offset{ 0.0f, 0.0f };
	Vector2 uv_scroll_size{ 1.0f, 1.0f };

	void SetAnimationIndex(int index, const std::vector<UVFrameAnimationDesc>& frame_descs);
	void Update(const std::vector<UVFrameAnimationDesc>& frame_descs);
	UVRect GetUVRect(const std::vector<UVFrameAnimationDesc>& frame_descs) const;
};