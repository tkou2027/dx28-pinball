#include "camera_editor.h"
//#include "component/render/component_camera.h"
#include "component/render/component_camera.h"
#include "global_context.h"
#include "platform/keyboard.h"
#include "config/camera_names.h"

void CameraEditor::Initialize()
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

void CameraEditor::Update()
{
	if (g_global_context.m_keyboard->IsKeyDown(KK_A))
	{
		RotateXZ(m_config.rotate_speed * GetDeltaTime());
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_D))
	{
		RotateXZ(-m_config.rotate_speed * GetDeltaTime());
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_Q))
	{
		const float zoom_speed = ComputeZoomSpeed();
		Zoom(zoom_speed * GetDeltaTime());
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_E))
	{
		const float zoom_speed = ComputeZoomSpeed();
		Zoom(-zoom_speed * GetDeltaTime());
	}
}

void CameraEditor::SetFocus(const Vector3& target, const Vector3& offset)
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetTarget(target);
	m_transform.SetPosition(target + offset);
}

void CameraEditor::Zoom(float distance)
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	const Vector3 camera_pos = m_transform.GetPosition();
	const Vector3 target_pos = camera.GetTarget();
	const Vector3 to_target = camera_pos - target_pos;
	float to_target_dist = to_target.Length();
	const auto& camera_conf = camera.GetShapeConfig();
	to_target_dist = Math::Clamp(
		to_target_dist + distance,
		camera_conf.z_near + Math::EPSILON,
		camera_conf.z_far - Math::EPSILON
	);
	const Vector3 to_target_dir = to_target.GetNormalized();
	const Vector3 camera_pos_new = target_pos + to_target_dir * to_target_dist;
	m_transform.SetPosition(camera_pos_new);
}

void CameraEditor::RotateXZ(float radius)
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	const Vector3 camera_pos = m_transform.GetPosition();
	const Vector3 target_pos = camera.GetTarget();
	const Vector3 to_target = camera_pos - target_pos;
	float radius_curr = atan2f(to_target.x, to_target.z);
	radius_curr += radius;
	const Vector3 to_target_xz = Vector3{ to_target.x, 0.0f, to_target.z };
	float to_target_xz_dist = to_target_xz.Length();
	const Vector3 to_target_new = Vector3{
		sinf(radius_curr) * to_target_xz_dist,
		to_target.y,
		cosf(radius_curr) * to_target_xz_dist
	};
	m_transform.SetPosition(target_pos + to_target_new);
}

float CameraEditor::ComputeZoomSpeed() const
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	const Vector3 camera_pos = m_transform.GetPosition();
	const Vector3 target_pos = camera.GetTarget();
	const Vector3 to_target = camera_pos - target_pos;
	float to_target_dist = to_target.Length();
	return Math::Clamp(
		to_target_dist * m_config.zoom_speed_factor,
		m_config.zoom_speed_min,
		m_config.zoom_speed_max
	);
}
