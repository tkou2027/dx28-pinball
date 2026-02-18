#pragma once
#include <string>
#include "render/config/camera_render_layer.h"

struct CameraPreset
{
	std::string name{};
	uint32_t width{};
	uint32_t height{};
	CameraRenderLayer render_layer{};
	int render_order{ 0 };
	int render_path_id{ 0 };
};

struct CameraPresets
{
	CameraPreset main{};
	CameraPreset reflect{};
	CameraPreset monitor{};
	CameraPreset screen_main{};
	CameraPreset screen_status{};
};

extern CameraPresets g_camera_presets;

class RenderPathManager;
namespace CameraPathConfig
{
	void InitializeRenderPath(RenderPathManager& render_path_manager);
}