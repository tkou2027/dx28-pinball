#pragma once
// TODO: move to each pass
#include <vector>
#include "math/vector3.h"
#include "math/vector4.h"

struct SSRSetting
{
	float max_distance = 6.0f;
	float stride = 5.0f;
	float thickness = 0.7f;
	int steps = 16;
};

struct ShaderSetting
{
	Vector3 light_direction{ 0.0f, -6.5f, -1.8f };
	Vector3 light_color{ 1.0f, 1.0f, 1.0f };
	float light_intensity{ 1.0f };

	int cel_offset{ 6 };
	
	SSRSetting ssr_setting{};
};

extern ShaderSetting g_shader_setting;