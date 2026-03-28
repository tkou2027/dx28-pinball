#pragma once

#include <string>
#include "math/vector3.h"
#include "camera_render_layer.h"

#include <array> // TODO
#include "math/camera_math.h"

enum class CameraType
{
	CAMERA_MAIN, // render to screen
	CAMERA,
	SHADOW,
	REFLECTION_CUBE,
	REFLECTION_PLANE,
};

struct CameraUsageConfig
{
	CameraType type{ CameraType::CAMERA_MAIN };
	CameraRenderLayer render_layer{ CameraRenderLayer::DEFAULT };
	int render_order{ 0 };
	// camera and texture key
	std::string render_camera_key{ "main" };
	// texture size
	uint32_t width{ 1280 };
	uint32_t height{ 720 };
	// render path
	int render_path_id{ 0 };
	// context
	bool invert_culling{ false };
	bool enable_frustum_culling{ false };
};

struct CameraPlaneReflectionConfig
{
	Vector3 plane_position{};
	Vector3 plane_normal{};
};

struct CameraTransformData
{
	Vector3 position{};
	Vector3 target{};
	Vector3 up{};
	// plane reflection // TODO: refactor ?
	CameraPlaneReflectionConfig reflection_plane{};
};

struct CameraViewData
{
	// object states
	Vector3 position{};
	// computed
	DirectX::XMFLOAT4X4 mat_proj{};
	DirectX::XMFLOAT4X4 mat_view{};
	// sprite
	bool has_sprite_projection{ false };
	DirectX::XMFLOAT4X4 mat_proj_sprite{};
	// screen size
	float screen_width{ 0.0f };
	float screen_height{ 0.0f };
};
