#pragma once
#include <vector>
#include "math/vector3.h"
#include "math/interval.h"
#include "math/transform3d.h"

class Circle;
class Cylinder;
class Shape;
struct ShapeHitInfo
{
	float t;
	Vector3 hit_position;
	Vector3 hit_normal;
};

class ShapeBase
{
public:
	virtual ~ShapeBase() = default;
	virtual void SetTransform(TransformNode3D& transform) = 0;
	Vector3 position{};
};

// circle on xz plane, for floors
class Circle : public ShapeBase
{
public:
	float radius{};
	void SetTransform(TransformNode3D& transform) override
	{
		position = transform.GetPositionGlobal();
	}
};

// back face of a circle, for stage border
class CylinderSideInverse : public ShapeBase
{
public:
	float radius{};
	float height{};
	Vector3 position{};
	void SetTransform(TransformNode3D& transform) override
	{
		position = transform.GetPositionGlobal();
	}
};

// vertical cylinder, for walls and characters
class Cylinder : public ShapeBase
{
public:
	float radius{};
	float height{};
	Vector3 position{};
	void SetTransform(TransformNode3D& transform) override
	{
		position = transform.GetPositionGlobal();
	}
};

// player
class Sphere : public ShapeBase
{
public:
	float radius{};
	Vector3 position{};
	void SetTransform(TransformNode3D& transform) override
	{
		position = transform.GetPositionGlobal();
	}
};

class Plane : public ShapeBase
{
public:
	Vector3 normal{};
	void SetTransform(TransformNode3D& transform) override
	{
		position = transform.GetPositionGlobal();
		normal = transform.GetForwardGlobal(); // using z as normal
	}
};

class PolygonCylinder : public ShapeBase
{
public:
	float radius{};
	float height{};
	float rotation_y{ 0.0f }; // radiant of y axis rotation
	int sides{ 3 };
	struct ClosestInfo
	{
		bool inside{ true };
		Vector3 closest_point{};
		Vector3 normal{};
	};
	void SetTransform(TransformNode3D& transform) override
	{
		position = transform.GetPositionGlobal();
		rotation_y = transform.GetRotationYGlobal();
		UpdateCached();
	}
	Vector3 GetSideNormal(int index) const;
	Vector2 GetPolygonVertexPos(int index) const;
	Vector2 GetPolygonVertexPosExpanded(int index, float expand) const;
	ClosestInfo GetClosestInfo(const Vector3& p) const;
	bool IfInSideFace(const Vector3& p, int index) const;
	bool IfInYFace(const Vector3& p) const;
private:
	void UpdateCached();
	std::vector<Vector2> m_side_vertices_xz;
	std::vector<Vector3> m_side_normals;
	float m_y_min{};
	float m_y_max{};
	float m_edge_length{};
};

class PolygonRing : public ShapeBase
{
public:
	float radius{};
	float radius_inner{};
	float rotation_y{ 0.0f }; // radiant of y axis rotation
	int sides{ 3 };
	void SetTransform(TransformNode3D& transform) override
	{
		position = transform.GetPositionGlobal();
		rotation_y = transform.GetRotationYGlobal();
	}
	Vector2 GetPolygonVertexPos(int index) const
	{
		// xz position of polygon vertex
		float rotation_step = Math::TWO_PI / static_cast<float>(sides);

		float rotation = rotation_step * index + rotation_y;
		// starts from x = 0, z = 1
		return {
			position.x + sinf(rotation) * radius,
			position.z + cosf(rotation) * radius
		};
	}
	Vector2 GetPolygonVertexPosInner(int index) const
	{
		// xz position of polygon vertex
		float rotation_step = Math::TWO_PI / static_cast<float>(sides);

		float rotation = rotation_step * index + rotation_y;
		// starts from x = 0, z = 1
		return {
			position.x + sinf(rotation) * radius_inner,
			position.z + cosf(rotation) * radius_inner
		};
	}

	Vector2 GetPolygonVertexPosCustom(int index, float custom_radius) const
	{
		// xz position of polygon vertex
		float rotation_step = Math::TWO_PI / static_cast<float>(sides);
		// starts from x = 0, z = 1
		float rotation = rotation_step * index + rotation_y;
		return {
			position.x + sinf(rotation) * custom_radius,
			position.z + cosf(rotation) * custom_radius
		};
	}

	std::vector<Vector2> GetOuterVertices(float expand) const
	{
		std::vector<Vector2> vertices;
		vertices.reserve(sides);
		float custom_radius = Math::Max(0.0f, radius + expand);
		for (int i = 0; i < sides; ++i)
		{
			vertices.push_back(GetPolygonVertexPosCustom(i, custom_radius));
		}
		return vertices;
	}

	std::vector<Vector2> GetInnerVertices(float expand) const
	{
		std::vector<Vector2> vertices;
		vertices.reserve(sides);
		float custom_radius = Math::Max(0.0f, radius_inner - expand);
		for (int i = 0; i < sides; ++i)
		{
			vertices.push_back(GetPolygonVertexPosCustom(i, custom_radius));
		}
		return vertices;
	}
};

//
//class Capsule {
//    Vector3 p1;
//	Vector3 p2;
//    float r;
//};


