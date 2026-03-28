#include "camera_screen_status.h"
#include "component/render/component_camera.h"
#include "config/camera_names.h"

void CameraScreenStatus::Initialize()
{
	auto& comp_camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	CameraUsageConfig config{};
	config.type = CameraType::CAMERA;
	auto& camera_preset = g_camera_presets.screen_status;
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;

	CameraShapeConfig shape{};
	shape.shape_type = CameraShapeType::ORTHOGRAPHIC;
	shape.aspect_ratio = static_cast<float>(config.width) / static_cast<float>(config.height);// 0.25f;
	shape.z_near = 0.0f;
	shape.z_far = 1.0f;
	shape.fov = 2.0f * atan(0.5f * config.height / shape.z_far);

	comp_camera.InitializeCamera(config, shape); //  TODO
	comp_camera.SetTarget({ 0.0f, 0.0f, 1.0f });
}
