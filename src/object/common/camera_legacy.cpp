#include "camera_legacy.h"
#include "config/constant.h"
#include "global_context.h"
#include "platform/keyboard.h"

#include "shader_setting.h"

using namespace DirectX;

void Camera::Initialize()
{
	m_config.screen_size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
	// m_transform.SetPosition({ 90.0f, 90.0f, 90.0f });
	// m_transform.SetPosition({ 0.0f, 50.0f, -50.0f });
	m_transform.SetPosition(Vector3{ 0.0f, 50.0f, -50.0f });
	m_config.target = Vector3{ 0.0f, 6.0f, 0.0f };
}

void Camera::Update()
{
	m_config.fov = g_shader_setting.camera_fov;

	Vector3 pos{ m_transform.GetPosition() };
	//if (g_global_context.m_keyboard->IsKeyDown(KK_Q))
	//{
	//	pos.x -= 0.1f;
	//	m_transform.SetPosition(pos);
	//}
	//if (g_global_context.m_keyboard->IsKeyDown(KK_E))
	//{
	//	pos.x += 0.1f;
	//	m_transform.SetPosition(pos);
	//}
	if (g_global_context.m_keyboard->IsKeyDown(KK_A))
	{
		float rotation = GetTargetRotation();
		rotation += 0.1f;
		SetTargetRotation(rotation);
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_D))
	{
		float rotation = GetTargetRotation();
		rotation -= 0.1f;
		SetTargetRotation(rotation);
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_W))
	{
		float rotation = GetTargetRotationVertical();
		rotation += 0.1f;
		SetTargetRotationVertical(rotation);
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_S))
	{
		float rotation = GetTargetRotationVertical();
		rotation -= 0.1f;
		SetTargetRotationVertical(rotation);
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_Q))
	{
		SetTargetZoom(-5.0f);
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_E))
	{
		SetTargetZoom(5.0f);
	}
	//if (g_global_context.m_keyboard->IsKeyDown(KK_LEFT))
	//{
	//	MoveTargetScreenX(-1.0f);
	//}
	//if (g_global_context.m_keyboard->IsKeyDown(KK_RIGHT))
	//{
	//	MoveTargetScreenX(1.0f);
	//}
	//if (g_global_context.m_keyboard->IsKeyDown(KK_UP))
	//{
	//	MoveTargetScreenY(1.0f);
	//}
	//if (g_global_context.m_keyboard->IsKeyDown(KK_DOWN))
	//{
	//	MoveTargetScreenY(-1.0f);
	//}
}

void Camera::SetConfig(const CameraConfig& config)
{
	m_config = config;
	m_transform.SetPosition(config.from);
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	const auto& camera_pos = m_transform.GetPosition();
	XMFLOAT3 up = XMFLOAT3(m_config.up.x, m_config.up.y, m_config.up.z);
	XMFLOAT3 target = XMFLOAT3(m_config.target.x, m_config.target.y, m_config.target.z);
	XMFLOAT3 pos = XMFLOAT3(camera_pos.x, camera_pos.y, camera_pos.z);
	//Vector3 focus = camera_pos + Vector3(0.0f, 0.0f, 15.0f);
	//XMFLOAT3 target = XMFLOAT3(focus.x, focus.y, focus.z);
    return XMMatrixLookAtLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up)
	);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	return XMMatrixPerspectiveFovLH(m_config.fov,
		static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT,
		m_config.z_near, m_config.z_far);
}

float Camera::GetTargetRotation() const
{
	Vector3 from_target = m_transform.GetPosition() - m_config.target;
	return atan2f(from_target.x, from_target.z);
}

void Camera::SetTargetRotation(float radiant)
{
	const Vector3& pos = m_transform.GetPosition();
	Vector3 from_center{ pos - m_config.target };
	float from_center_y = from_center.y;
	from_center.y = 0.0f;
	const float dist = from_center.Length();
	const Vector3 pos_rot{ dist * sinf(radiant), from_center_y, dist * cosf(radiant) };
	m_transform.SetPosition(m_config.target + pos_rot);
}

void Camera::SetTargetZoom(float delta_dist)
{
	const Vector3& pos = m_transform.GetPosition();
	Vector3 from_center{ pos - m_config.target };
	const float curr_dist = from_center.Length();
	if (Math::IsZero(curr_dist))
	{
		return;
	}
	float dist = Math::Max(1.0f, curr_dist + delta_dist);
	m_transform.SetPosition(m_config.target + from_center * (dist / curr_dist));
}

float Camera::GetTargetRotationVertical() const
{
	Vector3 from_target = m_transform.GetPosition() - m_config.target;
	float xz = Vector2{from_target.x, from_target.z}.Length();
	return atan2f(from_target.y, xz);
}

void Camera::SetTargetRotationVertical(float radiant)
{
	if (radiant >= Math::HALF_PI - 0.01f || radiant <= -Math::HALF_PI + 0.01f )
	{
		return;
	}
	const Vector3& pos = m_transform.GetPosition();
	Vector3 from_center{ pos - m_config.target };
	const float dist = from_center.Length();
	float y = dist * sinf(radiant);
	float xz = dist * cosf(radiant);
	float rotation_horizontal = GetTargetRotation();

	const Vector3 pos_rot{ xz * sinf(rotation_horizontal), y, xz * cosf(rotation_horizontal) };
	m_transform.SetPosition(m_config.target + pos_rot);
}

void Camera::MoveTargetScreenX(float delta_dist)
{
	const Vector3& pos = m_transform.GetPosition();
	Vector3 from_center{ pos - m_config.target };
	Vector3 direction = from_center.CrossProduct(m_config.up).GetNormalized();
	m_config.target = m_config.target + direction * delta_dist;
}

void Camera::MoveTargetScreenY(float delta_dist)
{
	const Vector3& pos = m_transform.GetPosition();
	const Vector3 from_center{ pos - m_config.target };
	const Vector3 direction = m_config.up.GetNormalized();
	m_config.target = m_config.target + direction * delta_dist;
}
