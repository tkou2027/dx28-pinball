#pragma once
#include <variant>
#include "math/shape.h"
#include "math/ray.h"

struct HitInfo
{
    bool hit{ false };
    float t{ -1 };
    float penetration{ -1.0f };
    Vector3 hit_normal{};
	Vector3 hit_position{};
    Vector3 movement{};
};

class ShapeCollider
{
public:
    using Variant = std::variant<Sphere, Plane, Circle, Cylinder, CylinderSideInverse, PolygonCylinder, PolygonRing>;
    ShapeCollider() : m_data(Circle{}) {};
    ShapeCollider(const Circle& shape) : m_data(shape) {};
    ShapeCollider(const Cylinder& shape) : m_data(shape) {};
    ShapeCollider(const CylinderSideInverse& shape) : m_data(shape) {};
    ShapeCollider(const PolygonCylinder& shape) : m_data(shape) {};
    ShapeCollider(const PolygonRing& shape) : m_data(shape) {};
    ShapeCollider(const Sphere& shape) : m_data(shape) {};
    ShapeCollider(const Plane& shape) : m_data(shape) {};

    // overlap with shape
    bool IfOverlap(const ShapeCollider& other) const;
    // movement
    bool IfCollideSwept(const ShapeCollider& other, const Vector3 movment, HitInfo& hit_info) const;
	// raycast, but assume ray is parallel to XZ plane
	bool IfRayHitXZ(const Ray& ray, float max_distance, HitInfo& hit_info) const;
    // collide with shape
    // bool IfCollide(const ShapeBase& other, ShapeHitInfo& hit_info) const;
    // set transform
    void SetTransform(TransformNode3D& transform);
    //bool IfOverlap(const Circle& circle) const;
    //bool IfOverlap(const Cylinder& cylinder) const;
	const Vector3& GetPosition() const;
private:
    Variant m_data{};
};