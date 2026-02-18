#pragma once
#include <cassert>
#include <DirectXMath.h>
#include "vector2.h"

class Vector3
{
public:
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };

	// constructors
	Vector3() = default;
	Vector3(float x_, float y_ = 0.0f, float z_ = 0.0f) : x(x_), y(y_), z(z_) {}
	Vector3(const Vector2& vec2) : x(vec2.x), y(vec2.y) {}

	float operator[](size_t i) const
	{
		assert(i < 3);
		return *(&x + i);
	}

	float& operator[](size_t i)
	{
		assert(i < 3);
		return *(&x + i);
	}

	Vector3 operator+(const Vector3& rhs) const
	{
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3& operator+=(const Vector3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3 operator-(const Vector3& rhs) const
	{
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3 operator*(float t) const
	{
		return Vector3{ x * t, y * t, z * t };
	}

	float Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float LengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	void Normalize()
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			return;
		}
		x /= length;
		y /= length;
		z /= length;
	}

	Vector3 GetNormalized() const
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			return Vector3{ 0.0f, 0.0f, 0.0f };
		}
		return Vector3{ x / length, y / length, z / length };
	}

	Vector3 CrossProduct(const Vector3& rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	static Vector3 Interpolate(const Vector3 from, const Vector3 to, float t)
	{
		return from * (1.0f - t) + to * t;
	}

	float Dot(const Vector3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	static float Dot(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	static Vector3 MoveTowards(const Vector3& form, const Vector3& to, float max_distance)
	{
		Vector3 delta{ to - form };
		float dist = delta.Length();
		if (dist <= max_distance || Math::IsZero(dist))
		{
			return to;
		}
		return form + delta * (max_distance / dist);
	}

	static Vector3 Reflect(const Vector3& v, const Vector3& n)
	{
		float dot = Vector3::Dot(v, n);
		return v - n * (2.0f * dot);
	}

	DirectX::XMFLOAT3 ToXMFLOAT3() const
	{
		return DirectX::XMFLOAT3{ x, y, z };
	}

	DirectX::XMVECTOR ToXMVECTOR(float w = 0.0f) const
	{
		return DirectX::XMVectorSet(x, y, z, w);
	}

	void LoadXMFLOAT3(const DirectX::XMFLOAT3& float3)
	{
		x = float3.x;
		y = float3.y;
		z = float3.z;
	}

	void LoadXMVECTOR(DirectX::XMVECTOR vec)
	{
		using namespace DirectX;
		XMFLOAT3 float3{};
		XMStoreFloat3(&float3, vec);
		x = float3.x;
		y = float3.y;
		z = float3.z;
	}

	static Vector3 FromXMVECTOR(DirectX::XMVECTOR vec)
	{
		Vector3 vec3{};
		vec3.LoadXMVECTOR(vec);
		return vec3;
	}
};
