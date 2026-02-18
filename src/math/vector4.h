#pragma once
#include <cassert>
#include <stddef.h>
#include <DirectXMath.h>

class Vector4
{
public:
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };
	float w{ 0.0f };

	// constructors
	Vector4() = default;
	Vector4(float x_, float y_ = 0.0f, float z_ = 0.0f, float w_ = 0.0f)
		: x(x_), y(y_), z(z_), w(w_) {}

	float operator[](size_t i) const
	{
		assert(i < 4);
		return *(&x + i);
	}

	float& operator[](size_t i)
	{
		assert(i < 4);
		return *(&x + i);
	}

	Vector4 operator+(const Vector4& rhs) const
	{
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	Vector4 operator-(const Vector4& rhs) const
	{
		return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	Vector4 operator*(float t) const
	{
		return Vector4{ x * t, y * t, z * t, w * t };
	}

	DirectX::XMFLOAT4 ToXMFLOAT4() const
	{
		return DirectX::XMFLOAT4{ x, y, z, w };
	}

	DirectX::XMVECTOR ToXMVECTOR() const
	{
		return DirectX::XMVectorSet(x, y, z, w);
	}

	void LoadXMFLOAT4(const DirectX::XMFLOAT4& float4)
	{
		x = float4.x;
		y = float4.y;
		z = float4.z;
		w = float4.w;
	}

	void LoadXMVECTOR(DirectX::XMVECTOR vec)
	{
		using namespace DirectX;
		XMFLOAT4 float4{};
		XMStoreFloat4(&float4, vec);
		x = float4.x;
		y = float4.y;
		z = float4.z;
		w = float4.w;
	}
};
