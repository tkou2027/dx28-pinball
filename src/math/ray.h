#pragma once
#include "math/vector3.h"

class Ray
{
public:
	Ray() {}
	Ray(const Vector3& origin, const Vector3& direction) : m_origin(origin), m_direction(direction) {}

	const Vector3& GetOrigin() const { return m_origin; }
	const Vector3& GetDirection() const { return m_direction; }

	Vector3 At(float t) const
	{
		return m_origin + m_direction * t;
	}
private:
	Vector3 m_origin{};
	Vector3 m_direction{ 1.0f,0.0f,0.0f };
};
