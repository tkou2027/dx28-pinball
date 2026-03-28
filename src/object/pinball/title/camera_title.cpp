#include "camera_title.h"
#include "component/render/component_camera.h"
#include "config/camera_names.h"

void CameraTitle::Initialize()
{
	auto& camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	CameraUsageConfig config{};
	config.type = CameraType::CAMERA_MAIN;
	auto& camera_preset = g_camera_presets.main;
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;
	camera.InitializeCamera(config, {});
}

void CameraTitle::SetTarget(const Vector3 target)
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetTarget(target);
}

void CameraTitle::SetCameraShapeConfig(const CameraShapeConfig& config)
{
	m_camera_shape_config = config;
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetShapeConfig(config);
}