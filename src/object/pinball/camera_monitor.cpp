#include "camera_monitor.h"
#include "component/render/component_camera.h"
#include "config/camera_names.h"

#include "object/game_object_list.h"
#include "object/pinball/player.h"

void CameraMonitor::Initialize()
{
	auto& comp_camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	CameraUsageConfig config{};
	config.type = CameraType::CAMERA;
	auto& camera_preset = g_camera_presets.monitor;
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;

	CameraShapeConfig shape{};
	shape.aspect_ratio = static_cast<float>(config.width) / static_cast<float>(config.height);// 0.25f;

	comp_camera.InitializeCamera(config, shape);
	m_transform.SetPosition({ 0.0f, 10.0f, -10.0f });
	comp_camera.SetTarget({ 0.0f, 0.0f, 0.0f });
}

void CameraMonitor::Update()
{
	auto player = GetOwner().FindGameObject<Player>();
	m_transform.SetPosition(player->GetTransform().GetPositionGlobal() + Vector3{ 0.0f, 5.0f, 5.0f });

	auto& comp_camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	comp_camera.SetTarget(player->GetTransform().GetPositionGlobal());
}
