#pragma once
#include <memory>
#include <vector>
#include "math/vector3.h"
#include "math/transform3d.h"
#include "shape_collider.h"
#include "component/component_id.h"
#include "physics/collider_layer.h"

class Collider
{
public:
	Collider();
	Collider(TransformNode3D* transform_parent, ColliderLayer::Type layer);
	// collision detection
	bool IfOverlap(const Collider& collider) const;
	bool IfCollideSwept(const Collider& other, const Vector3& movement, HitInfo& hit_info);
	bool IfRayHitXZ(const Ray& ray, float max_distance, HitInfo& hit_info) const;
	float GetDistance(const Vector3& pos, Vector3& out_min_position) const;

	void AddShape(const ShapeCollider& shape, TransformNode3D& shape_offset);
	void UpdateTransform();
	// getter setter
	bool GetActive() const { return m_active; }
	void SetActive(bool active) { m_active = active; }
	bool GetIsActor() const { return m_is_actor; }
	void SetIsActor(bool is_actor) { m_is_actor = is_actor;  }
	bool GetIsTrigger() const { return m_is_trigger; }
	void SetIsTrigger(bool is_trigger) { m_is_trigger = is_trigger; }

	void SetSnapReflection(bool snap, float step = DirectX::XM_PI / 6.0f)
	{
		m_snap_reflection = snap;
		m_snap_reflection_step = step;
	}
	bool GetSnapReflection() const { return m_snap_reflection; }
	float GetSnapReflectionStep() const { return m_snap_reflection_step; }

	// layer
	ColliderLayer::Type GetLayer() const { return m_layer; }
	void SetLayer(ColliderLayer::Type layer) { m_layer = layer; }
	// component / game object
	ComponentId GetComponentId() const { return m_component_id; }
	int GetIndexInComponent() const { return m_index_in_component; }
	std::weak_ptr<class GameObject> GetOwner() const { return m_object; }
	void SetOwner(std::weak_ptr<class GameObject> object, ComponentId id, size_t index)
	{
		m_object = object;
		m_component_id = id;
		m_index_in_component = index;
	}
	// transform
	const TransformNode3D* GetTransformParent() const { return m_transform_parent; }
	TransformNode3D* GetTransformParent() { return m_transform_parent; }
	void SetTransformParent(TransformNode3D* parent) { m_transform_parent = parent; }
	Vector3 velocity{};
	float bounce{ 0.1f };
	float gravity_scale{ 1.0f };
private:
	struct ColliderPart
	{
		ShapeCollider shape;
		TransformNode3D transform;
	};
	bool m_active{ true };
	bool m_is_actor{ false };
	bool m_is_trigger{ false };
	// make bumper reflection easier...
	bool m_snap_reflection{ false };
	float m_snap_reflection_step{ DirectX::XM_PI / 6.0f };

	ColliderLayer::Type m_layer{ ColliderLayer::Type::DEFAULT };
	TransformNode3D* m_transform_parent{ nullptr };
	// shape of collider
	std::vector<ColliderPart> m_parts;
	// accessing component
	ComponentId m_component_id{};
	int m_index_in_component{ -1 };
	// accessing object
	std::weak_ptr<class GameObject> m_object;
};
