#include "shape.h"

Vector3 PolygonCylinder::GetSideNormal(int index) const
{
	return m_side_normals[index % sides];
}

Vector2 PolygonCylinder::GetPolygonVertexPos(int index) const
{
	return m_side_vertices_xz[index % sides];
}

Vector2 PolygonCylinder::GetPolygonVertexPosExpanded(int index, float expand) const
{
	// xz position of polygon vertex
	float rotation_step = Math::TWO_PI / static_cast<float>(sides);
	float rotation = rotation_step * index + rotation_y;
	// starts from x = 0, z = 1
	return {
		position.x + sinf(rotation) * (radius + expand),
		position.z + cosf(rotation) * (radius + expand)
	};
}

PolygonCylinder::ClosestInfo PolygonCylinder::GetClosestInfo(const Vector3& p) const
{
	float closest_y = Math::Clamp(p.y, m_y_min, m_y_max);

	const Vector2 p_xz{ p.x, p.z };
	Vector2 closest_xz{ p.x, p.z };
	float min_dist_sq{ -1 };

	ClosestInfo info{};
	info.inside = p.y <= m_y_max && p.y >= m_y_min;
	if (Math::IsZero(m_edge_length))
	{
		info.closest_point = Vector3(closest_xz.x, closest_y, closest_xz.y);
		return info;
	}

	bool inside_xz = true;
	for (int i = 0; i < sides; ++i)
	{
		const Vector2& v0 = m_side_vertices_xz[i];
		const Vector2& v1 = m_side_vertices_xz[(i + 1) % sides];
		const Vector2 from_v0 = p_xz - v0;
		// notice side normal is vector3, use x and z
		float dot_normal = from_v0.x * m_side_normals[i].x + from_v0.y * m_side_normals[i].z;
		if (dot_normal > 0.0f)
		{
			inside_xz = false; // for top and bottom check
			info.inside = false;
		}

		const Vector2 edge = v1 - v0;
		float t = Math::Clamp(from_v0.Dot(edge) / (m_edge_length * m_edge_length), 0.0f, 1.0f);
		Vector2 p_on_edge = v0 + edge * t;
		float dist_sq = (p_xz - p_on_edge).LengthSquared();
		if (min_dist_sq < 0.0f || dist_sq < min_dist_sq)
		{
			min_dist_sq = dist_sq;
			closest_xz = p_on_edge;
			info.normal = m_side_normals[i];
		}
	}
	// top and bottom face
	if (inside_xz)
	{
		float dist_to_top = std::abs(p.y - m_y_max);
		float dist_to_bottom = std::abs(p.y - m_y_min);
		float min_dist = std::sqrt(min_dist_sq);

		info.closest_point = Vector3(closest_xz.x, p.y, closest_xz.y);
		if (p.y > m_y_max || (info.inside && dist_to_top < min_dist))
		{
			info.closest_point = Vector3(p.x, m_y_max, p.z);
			info.normal = Vector3(0, 1, 0);
			min_dist = dist_to_top;
		}
		if (p.y < m_y_min || (info.inside && dist_to_bottom < min_dist))
		{
			info.closest_point = Vector3(p.x, m_y_min, p.z);
			info.normal = Vector3(0, -1, 0);
			min_dist = dist_to_bottom;
		}
		return info;
	}

	info.closest_point = Vector3(closest_xz.x, closest_y, closest_xz.y);
	return info;
}

bool PolygonCylinder::IfInSideFace(const Vector3& p, int index) const
{
	if (p.y < m_y_min - Math::EPSILON || p.y > m_y_max + Math::EPSILON)
	{
		return false;
	}
	const Vector2& v0 = m_side_vertices_xz[index];
	const Vector2& v1 = m_side_vertices_xz[(index + 1) % sides];

	Vector2 edge = v1 - v0;
	Vector2 to_p = Vector2(p.x, p.z) - v0;
	float t = to_p.Dot(edge) / (m_edge_length * m_edge_length);
	return t >= -Math::EPSILON && t <= 1.0f + Math::EPSILON;
}

bool PolygonCylinder::IfInYFace(const Vector3& p) const
{
	Vector2 p_xz{ p.x, p.z };
	for (int i = 0; i < sides; ++i)
	{
		const Vector2& v0 = m_side_vertices_xz[i];
		const Vector2 from_v0 = p_xz - v0;
		float dot_normal = from_v0.x * m_side_normals[i].x + from_v0.y * m_side_normals[i].z;
		if (dot_normal > Math::EPSILON)
		{
			return false;
		}
	}
	return true;
}

void PolygonCylinder::UpdateCached()
{
	assert(sides >= 3);

	float half_h = height * 0.5f;
	m_y_min = position.y - half_h;
	m_y_max = position.y + half_h;

	if (m_side_vertices_xz.size() != static_cast<size_t>(sides))
	{
		m_side_vertices_xz.resize(sides);
		m_side_normals.resize(sides);
	}

	float rotation_step = Math::TWO_PI / static_cast<float>(sides);
	for (int i = 0; i < sides; ++i)
	{
		const float rotation = rotation_step * i + rotation_y;
		const float rotation_sin = sinf(rotation);
		const float rotation_cos = cosf(rotation);
		m_side_vertices_xz[i] = {
			position.x + rotation_sin * radius,
			position.z + rotation_cos * radius
		};
	}
	m_edge_length = (m_side_vertices_xz[0] - m_side_vertices_xz[1]).Length();
	for (int i = 0; i < sides; ++i)
	{
		int next_i = (i + 1) % sides;
		Vector2 edge = m_side_vertices_xz[next_i] - m_side_vertices_xz[i];
		m_side_normals[i] = Vector3{
			-edge.y,
			0.0f,
			edge.x
		};
		m_side_normals[i].Normalize();
	}
}
