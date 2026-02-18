#pragma once
#include <string>
//#include "render/config/model_geometry.h"
#include "uv_animation_desc.h"
#include "math/transform3d.h"

struct AnimationDesc
{
	std::string animation_file;
	int animation_id{ -1 };
	bool play_loop{ false };
};

struct ModelDesc
{
	// model
	std::string model_file;
	int model_id{ -1 };
	// geometry
	bool has_geometry{ false };
	int geometry_id{ -1 };
	// dynamic
	bool has_animation{ false };
	int model_skinning_id{ -1 };
	std::vector<AnimationDesc> animations_desc;
	// uv animation
	std::vector<UVFrameAnimationDesc> uv_animation_desc;
};

