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
		const float height = config.z_far * tan_half_fov * 2.0f;
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

DirectX::XMMATRIX CameraMath::CalculateProjectionMatrixOrthographic(float width, float height, float z_near, float z_far)
{
	return XMMatrixOrthographicLH(width, height, z_near, z_far);
}

DirectX::XMMATRIX CameraMath::CalculateProjectionMatrixOrthographicOffCenter(
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

// reference:yUnityz•½–Ê‚Ì‹¾–Ê”½ŽË‚ð“Ç‚Ý‰ð‚­
DirectX::XMMATRIX CameraMath::CalculateReflectionMatrix(const Vector3& plane_position, const Vector3& plane_normal)
{
	// plane ax + by + cz + d = 0
	const float a = plane_normal.x;
	const float b = plane_normal.y;
	const float c = plane_normal.z;
	const float d = -Vector3::Dot(plane_normal, plane_position); // -d = ax + by + cz = n dot p

	XMVECTOR plane_vector = XMVectorSet(a, b, c, d);
	// reflection matrix
	return XMMatrixReflect(plane_vector);
}

Vector4 CameraMath::CalculatePlaneInReflectionView(const DirectX::XMMATRIX& view_matrix, const Vector3& plane_position, const Vector3& plane_normal)
{
	float d = -Vector3::Dot(plane_normal, plane_position);
	XMVECTOR plane_vector_w = XMVectorSet(plane_normal.x, plane_normal.y, plane_normal.z, d);

	// use inverse transpose for rotation(plane)
	// as required by XMPlaneTransform api...
	XMMATRIX inv_trans_view = XMMatrixTranspose(XMMatrixInverse(nullptr, view_matrix));
	XMVECTOR plane_vector_v = XMPlaneTransform(plane_vector_w, inv_trans_view);

	Vector4 plane{};
	plane.LoadXMVECTOR(plane_vector_v);

	//if (plane.z > 0.0f)
	//{
	//	plane = plane * -1.0f;
	//}
	//plane.w += 0.001f;
	return plane;
}

// Oblique View Frustum Depth Projection and Clipping
// perry.cz/articles/ProjectionMatrix.xhtml
DirectX::XMMATRIX CameraMath::CalculateObliqueProjection(const DirectX::XMMATRIX& proj_matrix, const Vector4& plane_v)
{
	Vector4 q_clip{
		Math::Sign(plane_v.x),
		Math::Sign(plane_v.y),
		1.0f,
		1.0f
	};

	// same as using inverse of projection
	//XMFLOAT4X4 proj_matrix_float{};
	//XMStoreFloat4x4(&proj_matrix_float, proj_matrix);
	//Vector4 q_view{
	//	q_clip.x / proj_matrix_float._11,
	//	q_clip.y / proj_matrix_float._22,
	//	1.0f,
	//	(1.0f - proj_matrix_float._33) / proj_matrix_float._34
	//};
	//XMVECTOR q_view_vector = q_view.ToXMVECTOR();

	XMMATRIX inv_proj = XMMatrixInverse(nullptr, proj_matrix);
	XMVECTOR q_view_vector = XMVector4Transform(q_clip.ToXMVECTOR(), inv_proj);
	Vector4 q_view{};
	q_view.LoadXMVECTOR(q_view_vector);

	XMVECTOR plane_vector_v = plane_v.ToXMVECTOR();
	float dot = XMVectorGetX(XMVector4Dot(plane_vector_v, q_view_vector));
	float scale = q_view.z / dot;
	XMVECTOR scaled_plane = DirectX::XMVectorScale(plane_vector_v, scale);

	DirectX::XMMATRIX new_M = proj_matrix;
	// new_M.r[2] = scaled_plane; // wrong, set column for dx
	new_M.r[0].m128_f32[2] = scaled_plane.m128_f32[0];
	new_M.r[1].m128_f32[2] = scaled_plane.m128_f32[1];
	new_M.r[2].m128_f32[2] = scaled_plane.m128_f32[2];
	new_M.r[3].m128_f32[2] = scaled_plane.m128_f32[3];
	return new_M;
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
	XMMATRIX view_matrix = XMLoadFloat4x4(&view_matrix_float);
	XMMATRIX view_matrix_inv = XMMatrixInverse(nullptr, view_matrix);
	for (int i = 0; i < 8; ++i)
	{
		XMVECTOR corner_vec = corners[i].ToXMVECTOR();

		corner_vec = XMVector3TransformCoord(corner_vec, view_matrix_inv);
		Vector3 corner_world = Vector3::FromXMVECTOR(corner_vec);

		for (int axis = 0; axis < 3; ++axis) {
			min_p[axis] = Math::Min(min_p[axis], corner_world[axis]);
			max_p[axis] = Math::Max(max_p[axis], corner_world[axis]);
		}
	}

	return AABB(min_p, max_p);
}
