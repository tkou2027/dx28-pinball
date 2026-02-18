#include "camera_names.h"
#include "config/constant.h"

#include "render/render_path.h"
#include "render/path/render_path_main.h"
#include "render/path/render_path_monitor.h"
#include "render/path/render_path_screen_reflect.h"
#include "render/path/render_path_screen_content.h"

namespace
{
	CameraPreset g_preset_main
	{
		"main", // name
		SCREEN_WIDTH, // width
		SCREEN_HEIGHT, // height
		CameraRenderLayer::DEFAULT, // render_layer
		4 // render order
	};

	CameraPreset g_preset_reflect
	{
		"reflect", // name
		64, // width
		64, // height
		CameraRenderLayer::REFLECTED, // render_layer
		3 // render order
	};

	CameraPreset g_preset_monitor
	{
		"monitor", // name
		256, // width
		144, // height
		CameraRenderLayer::MONITOR, // render_layer
		0 // render order
	};

	CameraPreset g_preset_screen_main
	{
		"screen_main", // name
		1024, // width
		1024, // height
		CameraRenderLayer::SCREEN_MAIN, // render_layer
		2 // render order
	};

	CameraPreset g_preset_screen_status
	{
		"screen_status", // name
		1024, // width
		24, // height
		CameraRenderLayer::SCREEN_STATUS, // render_layer
		1 // render order
	};
}

CameraPresets g_camera_presets{};

void CameraPathConfig::InitializeRenderPath(RenderPathManager& render_path_manager)
{
	// main
	g_camera_presets.main = g_preset_main;
	g_camera_presets.main.render_path_id = render_path_manager.AddPath(std::make_unique<RenderPathMain>());

	// reflect
	g_camera_presets.reflect = g_preset_reflect;
	g_camera_presets.reflect.render_path_id = render_path_manager.AddPath(std::make_unique<RenderPathScreenReflect>());

	// monitor
	g_camera_presets.monitor = g_preset_monitor;
	g_camera_presets.monitor.render_path_id = render_path_manager.AddPath(std::make_unique<RenderPathMonitor>());

	int path_id_screen_content = render_path_manager.AddPath(std::make_unique<RenderPathScreenContent>());
	// screen main
	g_camera_presets.screen_main = g_preset_screen_main;
	g_camera_presets.screen_main.render_path_id = path_id_screen_content;
	// screen status
	g_camera_presets.screen_status = g_preset_screen_status;
	g_camera_presets.screen_status.render_path_id = path_id_screen_content;
}
