#pragma once
#include "component_collider.h"

// rigid body
class ComponentColliderActor : public ComponentCollider
{
public:
	void SetVelocity(const Vector3& velocity) { m_velocity = velocity; }
	const Vector3& GetVelocity() const { return m_velocity; }
	Vector3 GetVelocity() { return m_velocity; }
private:
	Vector3 m_velocity;
};