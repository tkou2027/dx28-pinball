#include "matrix4x4.h"
#include <cmath>

const Matrix4x4 Matrix4x4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

Matrix4x4::Matrix4x4(const Vector3& position, const Vector3& scale, float rotation_z)
{
	Matrix4x4 rotation = RotationZ(rotation_z);
	m_mat[0][0] = scale.x * rotation.m_mat[0][0];
	m_mat[0][1] = scale.y * rotation.m_mat[0][1];
	m_mat[0][2] = scale.z * rotation.m_mat[0][2];
	m_mat[0][3] = position.x;
	m_mat[1][0] = scale.x * rotation.m_mat[1][0];
	m_mat[1][1] = scale.y * rotation.m_mat[1][1];
	m_mat[1][2] = scale.z * rotation.m_mat[1][2];
	m_mat[1][3] = position.y;
	m_mat[2][0] = scale.x * rotation.m_mat[2][0];
	m_mat[2][1] = scale.y * rotation.m_mat[2][1];
	m_mat[2][2] = scale.z * rotation.m_mat[2][2];
	m_mat[2][3] = position.z;
	m_mat[3][0] = 0;
	m_mat[3][1] = 0;
	m_mat[3][2] = 0;
	m_mat[3][3] = 1;
}

Matrix4x4 Matrix4x4::OrthographicRightHanded(float width, float height, float near_clip, float far_clip)
{
	float depth = (far_clip - near_clip);
	// TODO: assert depth > epsilion
	return Matrix4x4(
		2.0f / width,
		0.0f,
		0.0f,
		0.0f,

		0.0f,
		2.0f / height,
		0.0f,
		0.0f,

		0.0f,
		0.0f,
		-1.0f / depth,
		0.0f,

		0.0f,
		0.0f,
		-near_clip / depth,
		1.0
	);
}
Matrix4x4 Matrix4x4::RotationZ(float radian)
{
	float sin_r = sinf(radian);
	float cos_r = cosf(radian);
	// left handed
	return Matrix4x4(
		cos_r, -sin_r, 0, 0,
		sin_r, cos_r, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}


