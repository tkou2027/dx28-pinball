#pragma once

#include "math/vector3.h"
#include "math/vector4.h"
#include "transform_node.h"

class Transform3D
{
public:
    DirectX::XMMATRIX GetMatrix() const
    {
        DirectX::XMMATRIX mat = DirectX::XMMatrixAffineTransformation(
            m_scale.ToXMVECTOR(),
            DirectX::XMVectorZero(), // rotation origin
            m_rotation.ToXMVECTOR(), // quaternion
            m_position.ToXMVECTOR() // translation
        );
        return mat;
    }

    DirectX::XMFLOAT4X4 GetFloat4x4() const
    {
        DirectX::XMFLOAT4X4 result;
        XMStoreFloat4x4(&result, GetMatrix());
        return result;
    }

	Vector3 m_position{ 0.0f, 0.0f, 0.0f };
    Vector3 m_scale{ 1.0f, 1.0f, 1.0f };
    Vector4 m_rotation{ 0.0f, 0.0f, 0.0f, 1.0f };
};

class TransformNode3D : public TransformNode
{
public:
	// set transform
	void SetPosition(const Vector3& pos);
	void SetScale(const Vector3& scale);
	void SetRotation(const Vector4& rotation);
	void SetRotationEuler(const Vector3& euler);
	// set / get utils
	void SetPositionDelta(const Vector3& pos_delta);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);
	void SetScale(float scale);
	void SetRotationYOnly (float yaw); // will set xz to 0!!!!
	void SetRotationYDelta (float yaw_delta);
	float GetRotationY() const;
	float GetRotationYGlobal();
	// get local
	const Vector3& GetPosition() const;
	const Vector3& GetScale() const;
	const Vector4& GetRotation() const;

	const Vector3 GetRight() const;
	const Vector3 GetUp() const;
	const Vector3 GetForward() const;
	// get global
	DirectX::XMMATRIX GetMatrixGlobal();
	DirectX::XMFLOAT4X4 GetFloat4x4Global();
	const Vector3 GetRightGlobal();
	const Vector3 GetUpGlobal();
	const Vector3 GetForwardGlobal();
	const Vector3 GetPositionGlobal();
private:
	void RecalculateCached();
	Transform3D m_transform_local{};
	DirectX::XMFLOAT4X4 m_matrix_cached{};
};