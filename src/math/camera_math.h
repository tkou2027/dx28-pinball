#pragma once
#include <DirectXMath.h>
#include "math/vector3.h"
#include "math/aabb.h"

enum class CameraShapeType
{
	PERSPECTIVE,
	ORTHOGRAPHIC,
	CUBE
};

struct CameraShapeConfig
{
	// shape
	CameraShapeType shape_type{ CameraShapeType::PERSPECTIVE };
	float z_near{ 1.0f };
	float z_far{ 500.0f };
	float fov{ 0.5f };
	float aspect_ratio{ 16.0f / 9.0f };
};

namespace CameraMath
{
	DirectX::XMMATRIX CalculateViewMatrix(
		const Vector3& position,
		const Vector3& target,
		const Vector3& up);
	void GetVectorsFromViewMatrix(
		const DirectX::XMMATRIX& view_matrix,
		Vector3& out_position,
		Vector3& out_target,
		Vector3& out_up
	);
	DirectX::XMMATRIX CalculateProjectionMatrix(const CameraShapeConfig& config);
	DirectX::XMMATRIX CalculateProjectionMatrixPerspective(
		float fov_y, float aspect_ratio, float z_near, float z_far);
	DirectX::XMMATRIX CalculateProjectionMatrixOrthographic(
		float width, float height, float z_near, float z_far);

	DirectX::XMMATRIX CalculateViewMatrixCube(int face_index, const Vector3& position);
	DirectX::XMMATRIX CalculateProjectionMatrixCube(float z_near, float z_far);

	void CalculateDirectionXZNormalized(
		const Vector3& position, const Vector3& target,
		Vector3& out_forward, Vector3& out_right);

	AABB CalculateFrustumAABBPerspective(
		float fov_y, float aspect_ratio, float z_near, float z_far, const DirectX::XMFLOAT4X4& view_matrix);


} // namespace CameraMath