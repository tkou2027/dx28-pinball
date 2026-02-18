#include "camera_follow.h"
#include "component/render/component_camera.h"
#include "config/camera_names.h"

namespace
{
	// normalize angle to [-pi, pi]
	static float NormalizeAngle(float a)
	{
		while (a > Math::PI - Math::EPSILON) { a -= Math::TWO_PI; }
		while (a <= -Math::PI + Math::EPSILON) { a += Math::TWO_PI; }
		return a;
	}
}

void CameraFollow::Initialize()
{
	auto& comp_camera = m_components.Add<ComponentCamera>(m_comp_id_camera);

	CameraUsageConfig config{};
	config.type = CameraType::CAMERA_MAIN;
	auto& camera_preset = g_camera_presets.main;
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;
	// shape
	m_camera_shape_config.fov = 0.45f;
	m_camera_shape_config.aspect_ratio = static_cast<float>(camera_preset.width) / static_cast<float>(camera_preset.height);
	comp_camera.InitializeCamera(config, m_camera_shape_config);
	m_follow_offset_current = m_follow_offset_fixed;
}

void CameraFollow::Update()
{
	if (m_follow_mode == FollowMode::STATIC)
	{
		return;
	}
	// follow position / rotation (compute m_follow_target and m_follow_offset_current)
	UpdateFollow();
	UpdateShake();

	// apply shake to target used by camera look
	Vector3 target_with_shake = m_follow_target + m_shake_state.shake_offset;

	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetTarget(target_with_shake);

	// place camera at target + current runtime offset
	m_transform.SetPosition(target_with_shake + m_follow_offset_current);

	// update camera shape
	camera.SetShapeConfig(m_camera_shape_config);
}

void CameraFollow::SetTargetObject(std::weak_ptr<GameObject> target, const Vector3& target_position_offset)
{
	m_target_object = target;
	m_target_position_offset = target_position_offset;
}

void CameraFollow::SetFollowFixed(const Vector3& offset)
{
	m_follow_mode = FollowMode::FIXED;
	m_follow_offset_fixed = offset;
	UpdateImmediate();
}

void CameraFollow::SetFollowCentered(const Vector3& center, const Vector3& offset)
{
	m_follow_mode = FollowMode::CENTERED;
	m_rotation_center = center;
	m_follow_offset_fixed = offset;
	UpdateImmediate();
}

void CameraFollow::SetFollowCentered(const Vector3& center)
{
	m_follow_mode = FollowMode::CENTERED;
	m_rotation_center = center;
	UpdateImmediate();
}

void CameraFollow::SetFollowCenteredTransfer(const Vector3& center)
{
	m_follow_mode = FollowMode::CENTERED;
	m_rotation_center = center;
}

void CameraFollow::SetShake(const CameraShakeConfig config)
{
	if (config.duration <= 0.0f)
	{
		return;
	}
	// TODO: override existing shake?
	m_shake_config = config;
	m_shake_state.shaking = true;
	m_shake_state.countdown = config.duration;
}

void CameraFollow::UpdateImmediate()
{
	UpdateFollowTarget(true);
	UpdateFollowOffset(true);

	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetTarget(m_follow_target);
	m_transform.SetPosition(m_follow_target + m_follow_offset_current);
}

void CameraFollow::UpdateFollow()
{
	UpdateFollowTarget();
	UpdateFollowOffset();
}

void CameraFollow::UpdateFollowTarget(bool immediate)
{
	// follow position (smooth target)
	auto target = m_target_object.lock();
	assert(target);
	const auto& desired_target = target->GetTransform().GetPositionGlobal() + m_target_position_offset;

	if (immediate)
	{
		m_follow_target = desired_target;
		return;
	}

	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	const auto& curr_target = camera.GetTarget();

	const Vector3 to_desired = desired_target - curr_target;
	float to_desired_dist = to_desired.Length();
	Vector3 moved_target{ desired_target };
	if (!Math::IsZero(to_desired_dist))
	{
		float smooth_speed = to_desired_dist * m_follow_config.smooth_factor;
		float frame_speed = Math::Clamp(smooth_speed, m_follow_config.min_speed, m_follow_config.max_speed);
		float movement = frame_speed * GetDeltaTime();
		if (to_desired_dist - movement > m_follow_config.max_delay_distance)
		{
			movement = to_desired_dist - m_follow_config.max_delay_distance;
		}
		const Vector3 move_step = to_desired.GetNormalized() * movement;

		if (move_step.Length() > to_desired_dist) {
			moved_target = desired_target;
		}
		else {
			moved_target = curr_target + move_step;
		}
	}
	m_follow_target = moved_target;
}

void CameraFollow::UpdateFollowOffset(bool immediate)
{
	if (m_follow_mode != FollowMode::CENTERED)
	{
		m_follow_offset_current = m_follow_offset_fixed;
		return;
	}
	auto target = m_target_object.lock();
	assert(target);
	const auto& desired_target = target->GetTransform().GetPositionGlobal() + m_target_position_offset;

	Vector3 to_center = m_rotation_center - desired_target;
	Vector2 to_center_xz{ to_center.x, to_center.z };
	float to_center_xz_length = to_center_xz.Length();
	if (Math::IsZero(to_center_xz_length))
	{
		// keep existing offset
		return;
	}
	float desired_yaw = std::atan2(to_center.x, to_center.z);

	Vector3 curr_cam_pos = m_transform.GetPosition();
	Vector3 curr_offset = curr_cam_pos - m_follow_target;
	Vector2 curr_offset_xz{ curr_offset.x, curr_offset.z };
	float curr_yaw{};
	if (Math::IsZero(curr_offset_xz.Length()))
	{
		curr_yaw = desired_yaw;
	}
	else
	{
		curr_yaw = std::atan2(-curr_offset.x, -curr_offset.z);
	}

	float new_yaw = curr_yaw;
	if (immediate)
	{
		new_yaw = desired_yaw;
	}
	else
	{
		// shortest angle diff
		float diff = NormalizeAngle(desired_yaw - curr_yaw);
		// smooth the yaw change (use smooth factor similarly to position smoothing)
		float dt = GetDeltaTime();
		float rotated = std::min(
			diff,
			m_follow_config.max_rotation_speed * dt);
		new_yaw = curr_yaw + rotated;

	}
	new_yaw = NormalizeAngle(new_yaw);
	Vector2 offset_fixed_xz{ m_follow_offset_fixed.x, m_follow_offset_fixed.z };
	float radius = offset_fixed_xz.Length();
	float new_x = std::sinf(new_yaw) * -radius;
	float new_z = std::cosf(new_yaw) * -radius;
	m_follow_offset_current = Vector3{ new_x, m_follow_offset_fixed.y, new_z };
}

void CameraFollow::UpdateShake()
{
	if (!m_shake_state.shaking)
	{
		return;
	}
	if (m_shake_state.countdown <= 0.0f)
	{
		m_shake_state.shaking = false;
		m_shake_state.shake_offset = Vector3{ 0.0f, 0.0f, 0.0f };
		m_shake_state.countdown = 0.0f;
		return;
	}
	m_shake_state.countdown -= GetDeltaTime();
	float t = 1.0f - (float)m_shake_state.countdown / m_shake_config.duration;
	float decay = 1.0f - t;
	// simple oscillation example (could be expanded)
	m_shake_state.shake_offset.x = std::sinf(t * 20.0f) * m_shake_config.strength.x * decay;
	m_shake_state.shake_offset.y = std::cosf(t * 12.0f) * m_shake_config.strength.y * decay;
	m_shake_state.shake_offset.z = std::sinf(t * 16.0f) * m_shake_config.strength.z * decay;
}

void CameraFollow::GetDirectionXZNormalized(Vector3& out_forward, Vector3& out_right) const
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.GetDirectionXZNormalized(out_forward, out_right);
}

void CameraFollow::SetCameraShapeConfig(const CameraShapeConfig& config)
{
	m_camera_shape_config = config;
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetShapeConfig(config);
}
