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
	REFLECTION
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
};

// TODO
//struct CameraConfig
//{
//	// shape
//	CameraShapeType shape_type{ CameraShapeType::PERSPECTIVE };
//	float z_near{ 1.0f };
//	float z_far{ 5000.0f };
//	float fov{ 0.5f };
//	float aspect_ratio{ 16.0f / 9.0f };
//
//	// usage
//	CameraType type{ CameraType::CAMERA_MAIN };
//	CameraRenderLayer render_layer{ CameraRenderLayer::DEFAULT };
//	int render_order{ 0 };
//	std::string render_texture_key;
//};

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

//struct CameraData
//{
//	CameraConfig config{};
//	// object states
//	Vector3 position{};
//	// computed
//	DirectX::XMFLOAT4X4 mat_proj{};
//	DirectX::XMFLOAT4X4 mat_view{};
//};

 //struct CameraDataCube
 //{
 //	CameraConfig config{};
 //	// object states
 //	Vector3 position{};
 //	// computed
 //	DirectX::XMFLOAT4X4 mat_proj{};
 //	std::array<DirectX::XMFLOAT4X4, 6> mat_views{};
 //};

// class CameraBase
// {
// public:
// 	virtual ~CameraBase() = default;
// 	virtual bool GetCameraData(std::vector<CameraData>& res) = 0;
// public:
// 	CameraConfig config;
// };
// 
// class CameraDefault : public CameraBase
// {
// public:
// 	bool GetCameraData(std::vector<CameraData>& res) override
// 	{
// 		CameraData camera_data
// 		{
// 			config,
// 			position,
// 			mat_proj,
// 			mat_views
// 		};
// 		res.push_back(camera_data);
// 		return true;
// 	}
// public:
// 	Vector3 position{};
// 	DirectX::XMFLOAT4X4 mat_proj{};
// 	DirectX::XMFLOAT4X4 mat_views{};
// };
// 
// class CameraCube : public CameraBase
// {
// public:
// 	bool GetCameraData(std::vector<CameraData>& res) override
// 	{
// 		for (int i = 0; i < 6; i++)
// 		{
// 			CameraData camera_data
// 			{
// 				config,
// 				position,
// 				mat_proj,
// 				mat_views[i]
// 			};
// 			res.push_back(camera_data);
// 		}
// 		return true;
// 	}
// public:
// 	Vector3 position{};
// 	DirectX::XMFLOAT4X4 mat_proj{};
// 	std::array<DirectX::XMFLOAT4X4, 6> mat_views{};
// };