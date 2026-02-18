#include "camera_math.h"

using namespace DirectX;

namespace {
	static Vector3 cube_ups[6]
	{
		{ 0.0f, 1.0f, 0.0f },  // +X
		{ 0.0f, 1.0f, 0.0f },  // -X
		{ 0.0f, 0.0f, -1.0f }, // +Y
		{ 0.0f, 0.0f, 1.0f },  // -Y
		{ 0.0f, 1.0f, 0.0f },  // +Z
		{ 0.0f, 1.0f, 0.0f }   // -Z
	};

	static Vector3 cube_looks[6]
	{
		{ 1.0f, 0.0f, 0.0f },  // +X
		{ -1.0f, 0.0f, 0.0f }, // -X
		{ 0.0f, 1.0f, 0.0f },  // +Y
		{ 0.0f, -1.0f, 0.0f }, // -Y
		{ 0.0f, 0.0f, 1.0f },  // +Z
		{ 0.0f, 0.0f, -1.0f }, // -Z
	};
}

DirectX::XMMATRIX CameraMath::CalculateViewMatrix(
	const Vector3& position,
	const Vector3& target,
	const Vector3& up)
{
	return XMMatrixLookAtLH(
		position.ToXMVECTOR(),
		target.ToXMVECTOR(),
		up.ToXMVECTOR()
	);
}

void CameraMath::GetVectorsFromViewMatrix(const DirectX::XMMATRIX& view_matrix, Vector3& out_position, Vector3& out_target, Vector3& out_up)
{
	DirectX::XMVECTOR det;
	DirectX::XMMATRIX inv_view = XMMatrixInverse(&det, view_matrix);
	DirectX::XMVECTOR pos_vec = inv_view.r[3];
	out_position = Vector3::FromXMVECTOR(pos_vec);
	DirectX::XMVECTOR look_dir = inv_view.r[2];
	look_dir = XMVectorNegate(look_dir);
	out_target = Vector3::FromXMVECTOR(XMVectorAdd(pos_vec, look_dir));
	DirectX::XMVECTOR up_vec = inv_view.r[1];
	out_up = Vector3::FromXMVECTOR(up_vec);
}

DirectX::XMMATRIX CameraMath::CalculateProjectionMatrix(const CameraShapeConfig& config)
{
	switch (config.shape_type)
	{
	case CameraShapeType::PERSPECTIVE:
	{
		return CalculateProjectionMatrixPerspective(
			config.fov, config.aspect_ratio, config.z_near, config.z_far);
	}
	case CameraShapeType::ORTHOGRAPHIC:
	{
		const float tan_half_fov = tanf(config.fov * 0.5f);
		const float height = config.z_near * tan_half_fov * 2.0f;
		const float width = height * config.aspect_ratio;
		return CalculateProjectionMatrixOrthographic(width, height, config.z_near, config.z_far);
	}
	}
}

DirectX::XMMATRIX CameraMath::CalculateProjectionMatrixPerspective(
	float fov_y, float aspect_ratio, float z_near, float z_far)
{
	return XMMatrixPerspectiveFovLH(fov_y, aspect_ratio, z_near, z_far);
}

DirectX::XMMATRIX CameraMath::CalculateProjectionMatrixOrthographic(
	float width, float height, float z_near, float z_far)
{
	return XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, z_near, z_far);
}

DirectX::XMMATRIX CameraMath::CalculateViewMatrixCube(int face_index, const Vector3& position)
{
	assert(face_index >= 0 && face_index < 6);
	const Vector3 target = position + cube_looks[face_index];
	const Vector3& up = cube_ups[face_index];
	return XMMatrixLookAtLH(
		position.ToXMVECTOR(),
		target.ToXMVECTOR(),
		up.ToXMVECTOR()
	);
}

DirectX::XMMATRIX CameraMath::CalculateProjectionMatrixCube(float z_near, float z_far)
{
	// fov PI/2, aspect ratio 1.0
	return XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.0f, z_near, z_far);
}

void CameraMath::CalculateDirectionXZNormalized(const Vector3& position, const Vector3& target, Vector3& out_forward, Vector3& out_right)
{
	Vector3 forward = target - position;
	forward.y = 0.0f;
	forward.Normalize();
	
	out_forward = forward;
	out_right = Vector3(forward.z, 0.0f, -forward.x);
}

AABB CameraMath::CalculateFrustumAABBPerspective(
	float fov_y, float aspect_ratio, float z_near, float z_far, const DirectX::XMFLOAT4X4& view_matrix_float)
{
	const float tan_half_fov_y = tanf(fov_y * 0.5f);
	float h_near_half = tan_half_fov_y * z_near;
	float h_far_half = tan_half_fov_y * z_far;
	float w_near_half = h_near_half * aspect_ratio;
	float w_far_half = h_far_half * aspect_ratio;

	Vector3 corners[8] = {
		// near plane
		{  w_near_half,  h_near_half, z_near },
		{ -w_near_half,  h_near_half, z_near },
		{  w_near_half, -h_near_half, z_near },
		{ -w_near_half, -h_near_half, z_near },
		// far plane
		{  w_far_half,   h_far_half,  z_far  },
		{ -w_far_half,   h_far_half,  z_far  },
		{  w_far_half,  -h_far_half,  z_far  },
		{ -w_far_half,  -h_far_half,  z_far  }
	};

	Vector3 min_p{ Math::INF, Math::INF, Math::INF };
	Vector3 max_p{ -Math::INF, -Math::INF, -Math::INF };

	for (int i = 0; i < 8; ++i)
	{
		XMVECTOR corner_vec = corners[i].ToXMVECTOR();
		XMMATRIX view_matrix = XMLoadFloat4x4(&view_matrix_float);
		corner_vec = XMVector3TransformCoord(corner_vec, view_matrix);
		Vector3 corner_world = Vector3::FromXMVECTOR(corner_vec);

		for (int axis = 0; axis < 3; ++axis) {
			min_p[axis] = Math::Min(min_p[axis], corner_world[axis]);
			max_p[axis] = Math::Max(max_p[axis], corner_world[axis]);
		}
	}

	return AABB(min_p, max_p);
}
