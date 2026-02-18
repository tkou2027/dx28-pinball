#pragma once

#include <vector>
#include "math/vector3.h"
#include "math/vector4.h"

struct EditorEnemySetting
{
	int type{ 0 };
	Vector3 position{  };
};

struct EditorLightSetting
{
	Vector3 position{ 0.0f, 2.0f, 0.0f };
	Vector3 color{ 1.0f, 1.0f, 1.0f };
};

struct SSRSetting
{
	float max_distance = 6.0f;
	float resolution = 1.0f;
	float thickness = 1.0f;
	int steps = 16;
};

struct ShaderSetting
{
	// model
	bool alicia{ false };
	// cel
	bool enable_cel{ true };
	bool enable_phong{ false };
	Vector3 light_direction{ 0.0f, -3.0f, -1.5f };
	Vector3 light_color{ 1.0f, 1.0f, 1.0f };
	float light_intensity{ 1.0f };

	int cel_offset{ 6 };
	bool enable_glow{ true };
	// outline
	bool enable_outline{ true };
	Vector4 outline_color{ 19.0f / 256.0f, 23.0f / 256.0f, 86.0f / 256.0f, 1.0f };//{ 0.384f, 0.165f, 0.059f, 1.0f };
	float outline_width{ 0.003f };
	// camera
	float camera_fov{ 0.5f };
	// mask
	Vector3 mask_position{ 0.0f, 6.0f, 15.0f };
	float mask_yaw{ Math::PI * 0.25f };
	float mask_scale{ 4.0f };
	bool mask_animation{ false };
	// aim
	Vector3 aim_position{ 0.0f, 2.0f, 30.0f };
	float aim_yaw{ Math::PI * 0.25f };
	// buttons
	bool desired_dash{ false };
	bool desired_reset{ false };

	// map
	int map_width_half{ 5 };
	int map_depth_half{ 5 };

	// enemy
	//std::vector<EditorEnemySetting> enemies
	//{
	//	{ 1, { 0.0f, 0.0f, 50.0f }},
	//	{ 0, { 50.0f, 0.0f, 0.0f }},
	//	{ 0, { -50.0f, 0.0f, 0.0f }},
	//	{ 0, { 40.0f, 0.0f, 20.0f }},
	//	{ 0, { -40.0f, 0.0f, 20.0f }},
	//	{ 0, { 40.0f, 0.0f, -20.0f }},
	//	{ 0, { -40.0f, 0.0f, -20.0f }},
	//};
	std::vector<EditorEnemySetting> enemies
	{
		{ 1, { 0.0f, 0.0f, 50.0f }},
		{ 0, { 40.0f, 0.0f, 0.0f }},
		{ 0, { -40.0f, 0.0f, 0.0f }},
		{ 0, { 40.0f, 0.0f, 20.0f }},
		{ 0, { -40.0f, 0.0f, 20.0f }},
		{ 0, { 40.0f, 0.0f, -20.0f }},
		{ 0, { -40.0f, 0.0f, -20.0f }},
		{ 0, { 40.0f, 0.0f, -40.0f }},
		{ 0, { -40.0f, 0.0f, -40.0f }},
	};
	bool desired_enemy_reset{ true };

	std::vector<EditorLightSetting> lights
	{
		{ {20.0f, 2.0f, 0.0f}, { 1.0f, 0.5f, 1.0f } },
		{ {-20.0f, 2.0f, 0.0f}, { 0.5f, 1.0f, 1.0f } },
	};

	SSRSetting ssr_setting{};
};

extern ShaderSetting g_shader_setting;