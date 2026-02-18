#include "camera_reflect.h"
#include "component/render/component_camera.h"
#include "config/camera_names.h"

#include "object/game_object_list.h"
#include "object/pinball/player.h"

void CameraReflect::Initialize()
{
	auto& comp_camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	CameraUsageConfig config{};
	config.type = CameraType::REFLECTION;
	config.render_layer = g_camera_presets.reflect.render_layer; // TOOD
	config.render_order = g_camera_presets.reflect.render_order;
	config.render_camera_key = g_camera_presets.reflect.name; // TOOD
	config.width = g_camera_presets.reflect.width;
	config.height = g_camera_presets.reflect.height;
	config.render_path_id = g_camera_presets.reflect.render_path_id;

	CameraShapeConfig shape{};
	shape.aspect_ratio = 1.0f;

	comp_camera.InitializeCamera(config, shape); //  TODO
	m_transform.SetPosition({ 0.0f, 24.0f, 0.0f });
}
