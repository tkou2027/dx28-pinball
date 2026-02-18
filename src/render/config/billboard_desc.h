#pragma once
#include <vector>
#include "uv_animation_desc.h"
#include "texture_resource_id.h"

struct BillboardDesc
{
	TextureResourceId texture_id{ -1 };
	bool has_uv_animation{ false };
	std::vector<UVFrameAnimationDesc> uv_animation_desc;
};