#pragma once
#include <cassert>
#include <cmath>
#include <DirectXMath.h>

#include "math.h"
class Vector2
{
public:
	float x{ 0.0f };
	float y{ 0.0f };

	// constructors
	Vector2() = default;
	Vector2(float x_, float y_) : x(x_), y(y_) {}

	float operator[](size_t i) const
	{
		assert(i < 2);
		return i == 0 ? x : y;
	}

	float& operator[](size_t i)
	{
		assert(i < 2);
		return i == 0 ? x : y;
	}

	Vector2 operator+(const Vector2& rhs) const
	{
		return Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 operator-(const Vector2& rhs) const
	{
		return Vector2(x - rhs.x, y - rhs.y);
	}

	Vector2 operator*(float t) const
	{
		return Vector2{ x * t, y * t };
	}

	float Length() const
	{
		return sqrtf(x * x + y * y);
	}

	float LengthSquared() const
	{
		return x * x + y * y;
	}

	void Normalize()
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			x = 0.0f;
			y = 0.0f;
			return;
		}
		x = x / length;
		y = y / length;
	}

	Vector2 GetNormalized() const
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			return Vector2{ 0.0f, 0.0f };
		}
		return Vector2{ x / length, y / length };
	}

	Vector2 GetNormalized(float length) const
	{
		if (length < Math::EPSILON)
		{
			return Vector2{ 0.0f, 0.0f };
		}
		return Vector2{ x / length, y / length };
	}

	void ClampMagnitude(float max_length)
	{
		if (max_length < Math::EPSILON)
		{
			x = 0.0f;
			y = 0.0f;
			return;
		}
		x = x / max_length;
		y = y / max_length;
	}
	static Vector2 Interpolate(const Vector2& from, const Vector2& to, float t)
	{
		return from * (1.0f - t) + to * t;
	}
	float Dot(const Vector2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}
	static float Dot(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	DirectX::XMFLOAT2 ToXMFLOAT2() const
	{
		return DirectX::XMFLOAT2{ x, y };
	}
};
