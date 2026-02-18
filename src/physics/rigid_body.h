#pragma once
#include "collider.h"

class RigidBody
{
public:
	// collider
	Collider& GetCollider() { return m_collider; }
	const Collider& GetCollider() const { return m_collider; }
	// velocity
	void SetVelocity(const Vector3& velocity) { m_velocity = velocity; }
	const Vector3& GetVelocity() const { return m_velocity; }
	Vector3 GetVelocity() { return m_velocity; }
	// transform
	void SetParentTransform(TransformNode3D* parent_transform) { m_parent_transform = parent_transform; }
	TransformNode3D& GetParentTransform() { return *m_parent_transform; }
	const TransformNode3D& GetParentTransform() const { return *m_parent_transform; }
private:
	Collider m_collider{};
	Vector3 m_velocity{};
	TransformNode3D* m_parent_transform{ nullptr };
};