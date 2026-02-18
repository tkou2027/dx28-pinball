#include "component_collider.h"

void ComponentCollider::Update()
{
	for (auto& collider : m_colliders)
	{
		collider.UpdateTransform();
	}
}

int ComponentCollider::AddCollider(const Collider& collider)
{
	int index = m_colliders.size();
	m_colliders.push_back(collider);
	m_colliders[index].SetOwner(m_object, GetId(), index);
	return index;
}

Collider& ComponentCollider::GetCollider(int index)
{
	return  m_colliders.at(index);
}

const Collider& ComponentCollider::GetCollider(int index) const
{
	return  m_colliders.at(index);
}
