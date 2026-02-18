#include "transform3d.h"
#include "transform2d.h"
using namespace DirectX;

namespace
{

}

void TransformNode3D::SetPosition(const Vector3& pos)
{
	m_transform_local.m_position = pos;
	MarkDirty();
}

void TransformNode3D::SetScale(const Vector3& scale)
{
	m_transform_local.m_scale = scale;
	MarkDirty();
}

void TransformNode3D::SetRotation(const Vector4& rotation)
{
	m_transform_local.m_rotation = rotation;
	MarkDirty();
}

void TransformNode3D::SetRotationEuler(const Vector3& euler)
{
	XMVECTOR q = XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z);
	m_transform_local.m_rotation.LoadXMVECTOR(q);
	MarkDirty();
}

void TransformNode3D::SetPositionDelta(const Vector3& pos_delta)
{
	SetPosition(m_transform_local.m_position + pos_delta);
}

void TransformNode3D::SetPositionX(float x)
{
	SetPosition({ x, m_transform_local.m_position.y, m_transform_local.m_position.z });
}
void TransformNode3D::SetPositionY(float y)
{
	SetPosition({ m_transform_local.m_position.x, y, m_transform_local.m_position.z });
}

void TransformNode3D::SetPositionZ(float z)
{
	SetPosition({ m_transform_local.m_position.x, m_transform_local.m_position.y, z });
}

void TransformNode3D::SetScale(float scale)
{
	SetScale(Vector3{ scale, scale, scale });
}

void TransformNode3D::SetRotationYOnly(float yaw)
{
	const auto y_axis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const auto q = DirectX::XMQuaternionRotationAxis(y_axis, yaw);
	m_transform_local.m_rotation.LoadXMVECTOR(q);
}

void TransformNode3D::SetRotationYDelta(float yaw_delta)
{
	const auto current_q = m_transform_local.m_rotation.ToXMVECTOR();
	// rotation around y axis
	const auto y_axis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const auto delta_q = DirectX::XMQuaternionRotationAxis(y_axis, yaw_delta);
	const auto result_q = DirectX::XMQuaternionMultiply(current_q, delta_q);
	m_transform_local.m_rotation.LoadXMVECTOR(result_q);
}

float TransformNode3D::GetRotationY() const
{
	// wiki - Conversion_between_quaternions_and_Euler_angles Quaternion to Euler angles
	const auto& q = m_transform_local.m_rotation;
	const float sin_y_cosp = 2 * (q.w * q.z + q.x * q.y);
	const float cos_y_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	const float yaw = std::atan2(sin_y_cosp, cos_y_cosp);
	return yaw;
}

float TransformNode3D::GetRotationYGlobal()
{
	if (!m_parent)
	{
		return GetRotationY();
	}
	auto forward = GetForwardGlobal();
	return std::atan2(forward.x, forward.z);
}

const Vector3& TransformNode3D::GetPosition() const
{
	return m_transform_local.m_position;
}
const Vector3& TransformNode3D::GetScale() const
{
	return m_transform_local.m_scale;
}
const Vector4& TransformNode3D::GetRotation() const
{
	return m_transform_local.m_rotation;
}

const Vector3 TransformNode3D::GetRight() const
{
	auto mat = m_transform_local.GetMatrix();
	return Vector3::FromXMVECTOR(mat.r[0]).GetNormalized();
	// TODO: cache?
}

const Vector3 TransformNode3D::GetUp() const
{
	auto mat = m_transform_local.GetMatrix();
	return Vector3::FromXMVECTOR(mat.r[1]).GetNormalized();
	// TODO: cache?
}

const Vector3 TransformNode3D::GetForward() const
{
	auto mat = m_transform_local.GetMatrix();
	return Vector3::FromXMVECTOR(mat.r[2]).GetNormalized();
	// TODO: cache?
}

XMMATRIX TransformNode3D::GetMatrixGlobal()
{
	if (m_dirty)
	{
		RecalculateCached();
	}
	return XMLoadFloat4x4(&m_matrix_cached);
}

XMFLOAT4X4 TransformNode3D::GetFloat4x4Global()
{
	if (m_dirty)
	{
		RecalculateCached();
	}
	return m_matrix_cached;
}

const Vector3 TransformNode3D::GetRightGlobal()
{
	if (m_dirty)
	{
		RecalculateCached();
	}
	auto mat = XMLoadFloat4x4(&m_matrix_cached);
	return Vector3::FromXMVECTOR(mat.r[0]).GetNormalized();
}

const Vector3 TransformNode3D::GetUpGlobal()
{
	if (m_dirty)
	{
		RecalculateCached();
	}
	auto mat = XMLoadFloat4x4(&m_matrix_cached);
	return Vector3::FromXMVECTOR(mat.r[1]).GetNormalized();
}

const Vector3 TransformNode3D::GetForwardGlobal()
{
	if (m_dirty)
	{
		RecalculateCached();
	}
	auto mat = XMLoadFloat4x4(&m_matrix_cached);
	return Vector3::FromXMVECTOR(mat.r[2]).GetNormalized();
}

const Vector3 TransformNode3D::GetPositionGlobal()
{
	if (m_dirty)
	{
		RecalculateCached();
	}
	auto mat = XMLoadFloat4x4(&m_matrix_cached);
	return Vector3::FromXMVECTOR(mat.r[3]);
	// TODO: ???
}

void TransformNode3D::RecalculateCached()
{
	if (auto parent = m_parent)
	{
		XMMATRIX matrix_parent = dynamic_cast<TransformNode3D*>(parent)->GetMatrixGlobal();
		XMMATRIX matrix_local = m_transform_local.GetMatrix();
		XMMATRIX matrix_world = XMMatrixMultiply(matrix_local, matrix_parent);
		XMStoreFloat4x4(&m_matrix_cached, matrix_world);
	}
	else
	{
		m_matrix_cached = m_transform_local.GetFloat4x4();
	}
}