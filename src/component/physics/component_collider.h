#pragma once
#include "component/component.h"
#include "physics/collider.h"

class ComponentCollider : public Component
{
public:
	virtual ~ComponentCollider() = default;
	void Update() override;
	int AddCollider(const Collider& collider);
	Collider& GetCollider(int index);
	const Collider& GetCollider(int index) const;
	const std::vector<Collider>& GetColliders() const { return m_colliders; }
	std::vector<Collider>& GetColliders() { return m_colliders; }
protected:
	std::vector<Collider> m_colliders;
};