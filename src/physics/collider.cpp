#include "collider.h"

Collider::Collider() = default;
Collider::Collider(TransformNode3D* transform_parent, ColliderLayer::Type layer)
	: m_transform_parent(transform_parent), m_layer(layer)
{
}

bool Collider::IfOverlap(const Collider& other) const
{
	for (const auto& part_a : m_parts)
	{
		for (const auto& part_b : other.m_parts)
		{
			if (part_a.shape.IfOverlap(part_b.shape))
			{
				return true;
			}
		}
	}
	return false;
}

bool Collider::IfCollideSwept(const Collider& solid, const Vector3& movement, HitInfo& hit_info)
{
	HitInfo hit_info_tmp{};
	for (const auto& part_actor : m_parts)
	{
		for (const auto& part_solid : solid.m_parts)
		{
			if (part_actor.shape.IfCollideSwept(part_solid.shape, movement, hit_info_tmp))
			{
				if (hit_info_tmp.t > hit_info.t || !hit_info.hit)
				{
					hit_info = hit_info_tmp; // copy
				}
			}
		}
	}
	return hit_info.hit;
}

bool Collider::IfRayHitXZ(const Ray& ray, float max_distance, HitInfo& hit_info) const
{
	for (const auto& part : m_parts)
	{
		HitInfo hit_info_tmp{};
		if (part.shape.IfRayHitXZ(ray, max_distance, hit_info_tmp))
		{
			if (!hit_info.hit || hit_info_tmp.t < hit_info.t)
			{
				hit_info = hit_info_tmp; // copy
			}
		}
	}
	return hit_info.hit;
}

float Collider::GetDistance(const Vector3& origin, Vector3& out_min_position) const
{
	float min_distance = std::numeric_limits<float>::max();
	for (const auto& part : m_parts)
	{
		HitInfo hit_info_tmp{};
		const Vector3& position = part.shape.GetPosition();
		float distance = (position - origin).Length();
		if (distance < min_distance)
		{
			min_distance = distance;
			out_min_position = position;
		}
	}
	return min_distance;
}

void Collider::AddShape(const ShapeCollider& shape, TransformNode3D& shape_offset)
{
	ShapeCollider copy{ shape };
	copy.SetTransform(shape_offset);
	m_parts.push_back({ copy, shape_offset });
}

void Collider::UpdateTransform()
{
	// TODO: lazy
	for (auto& part : m_parts)
	{
		part.shape.SetTransform(part.transform);
	}
}