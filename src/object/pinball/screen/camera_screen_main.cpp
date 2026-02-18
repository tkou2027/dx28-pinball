#include "camera_screen_main.h"
#include "component/render/component_camera.h"
#include "config/camera_names.h"

void CameraScreenMain::Initialize()
{
	auto& comp_camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	CameraUsageConfig config{};
	config.type = CameraType::CAMERA;
	auto& camera_preset = g_camera_presets.screen_main;
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;

	CameraShapeConfig shape{};
	shape.shape_type = CameraShapeType::ORTHOGRAPHIC;
	shape.aspect_ratio = static_cast<float>(config.width) / static_cast<float>(config.height);// 0.25f;

	comp_camera.InitializeCamera(config, shape); //  TODO
	comp_camera.SetTarget({ 0.0f, 0.0f, 1.0f });
}
