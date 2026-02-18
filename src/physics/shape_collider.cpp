#include "shape_collider.h"

namespace
{
	// 2d utils
	struct HitInfo2D
	{
		bool hit{ false };
		float t{};
		// Vector2 hit_position{};
		Vector2 hit_normal{};
	};
	// return t relative to ray segment
	bool IntersectSegments2D(
		Vector2 ray_start, Vector2 ray_end,
		Vector2 seg_start, Vector2 seg_end,
		HitInfo2D& hit_info)
	{
		Vector2 ray_dir = ray_end - ray_start;
		Vector2 seg_dir = seg_end - seg_start;

		float cross = ray_dir.x * seg_dir.y - ray_dir.y * seg_dir.x;
		// parallel
		if (Math::IsZero(cross))
		{
			return false;
		}
		// ray_start + t * ray_dir = seg_start + u * seg_dir
		Vector2 qp = seg_start - ray_start;
		float t = (qp.x * seg_dir.y - qp.y * seg_dir.x) / cross;
		float u = (qp.x * ray_dir.y - qp.y * ray_dir.x) / cross;


		if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
			hit_info.hit = true;
			hit_info.t = t;
			// hit normal, to out side(-z, x)
			Vector2 n = Vector2(-seg_dir.y, seg_dir.x).GetNormalized();
			if (Vector2::Dot(n, ray_dir) > 0)
			{
				n = n * -1.0f; // ?
			}
			// hit_info.hit_position = ray_start + ray_dir * t;
			hit_info.hit_normal = n;
			return true;
		}
		return false;
	}

	bool IntersectSegmentCircle2D(
		Vector2 ray_start, Vector2 ray_end,
		Vector2 circle_center, float circle_radius,
		HitInfo2D& hit_info
	)
	{
		Vector2 ray_dir = ray_end - ray_start;
		Vector2 from_center = ray_start - circle_center;

		// (from_center + ray_dir * t)^2 = r^2
		//  ray_dir^2 t^2 + 2Dot(from_center, ray_dir) + from_center^2  - r^2 = 0
		// a*t^2 + 2*b*t + c = 0
		float a = ray_dir.LengthSquared();
		if (Math::IsZero(a))
		{
			return false; // ?
		}
		float b = from_center.Dot(ray_dir);
		float c = from_center.LengthSquared() - circle_radius * circle_radius;
		float delta = b * b - a * c; // * 4
		if (Math::IsZero(delta))
		{
			delta = 0.0f;
		}
		if (delta < 0.0f)
		{
			return false;
		}
		float sqrt_disc = std::sqrt(delta);
		float t = (-b - sqrt_disc) / a; // smaller t
		if (t >= 0.0f && t <= 1.0f)
		{
			hit_info.hit = true;
			hit_info.t = t;
			Vector2 hit_p = ray_start + ray_dir * t;
			// hit_info.hit_position = hit_p;
			hit_info.hit_normal = (hit_p - circle_center).GetNormalized();
			return true;
		}
		return false;
	}

	// util for testing height
	bool IfSizeOverlap(float a_center, float a_size, float b_center, float b_size)
	{
		float a_size_half = a_size * 0.5f;
		float b_size_half = b_size * 0.5f;
		return (a_center + a_size_half) >= (b_center - b_size_half)
			&& (b_center + b_size_half) >= (a_center - a_size_half);
	}

	bool IfSizeOverlap(float a_center, float a_size, float a_movement, float b_center, float b_size)
	{
		float a_size_half = a_size * 0.5f;
		float b_size_half = b_size * 0.5f;

		// expand a by movement
		float a_min = Math::Min(a_center, a_center + a_movement) - a_size_half;
		float a_max = Math::Max(a_center, a_center + a_movement) + a_size_half;

		return a_max >= (b_center - b_size_half)
			&& (b_center + b_size_half) >= a_min;
	}

	struct ShapeOverlap {
		bool operator()(const Circle& a, const Circle& b) const {
			return (a.position - b.position).LengthSquared() < (a.radius + b.radius) * (a.radius + b.radius);
		}
		bool operator()(const Circle& a, const Cylinder& b) const {
			return false;
		}
		bool operator()(const Cylinder& cy, const Circle& ci) const {
			return (*this)(ci, cy);
		}
		template <typename T, typename U>
		bool operator()(const T&, const U&) const {
			return false;
		}
	};

	struct ShapeCollideSwept {
		bool operator()(const Cylinder& cy, const Circle& ci, const Vector3& movement, HitInfo& hit_info) const
		{

			if (!IfSizeOverlap(cy.position.y, cy.height, movement.y, ci.position.y, 0.0f))
			{
				return false;
			}
			//float normal_y = 1.0f;


			float half_h = cy.height * 0.5f;
			float from_ci_y = cy.position.y - ci.position.y;
			float from_ci_top = from_ci_y + half_h;
			float from_ci_bottom = from_ci_y - half_h;

			float t_y = -1.0f;
			float normal_y = 0.0f;

			// moving up
			if (movement.y > 0.0f)
			{
				// top under circle
				if (from_ci_top <= 0.0f)
				{
					t_y = -from_ci_top / movement.y;
					normal_y = -1.0f; // normal down
				}
			}
			else if (movement.y < 0.0f)
			{
				// bottom above circle
				if (from_ci_bottom >= 0.0f)
				{
					t_y = -from_ci_bottom / movement.y;
					normal_y = 1.0f; // normal up
				}
			}

			// no hit, return
			if (t_y < 0.0f || t_y > 1.0f)
			{
				return false;
			}

			// test xz
			Vector3 pos_at_t = cy.position + movement * t_y;
			float dx = pos_at_t.x - ci.position.x;
			float dz = pos_at_t.z - ci.position.z;
			float dist_sq_xz = dx * dx + dz * dz;
			float radius_sum = cy.radius + ci.radius;

			if (dist_sq_xz <= radius_sum * radius_sum) {
				// hit
				hit_info.hit = true;
				hit_info.t = t_y;
				hit_info.hit_normal = Vector3(0, normal_y, 0);
				hit_info.hit_position = pos_at_t;
				// hit_info.movement = {movement.x, 0.0f, movement.z}; // TODO

				Vector3 movement_canceled = movement * (1.0f - t_y);
				Vector3 movement_proj
					= movement - hit_info.hit_normal * Vector3::Dot(movement_canceled, hit_info.hit_normal);
				hit_info.movement = movement_proj;

				return true;
			}

			return false;
		}

		bool operator()(const Cylinder& cy, const PolygonRing& ci, const Vector3& movement, HitInfo& hit_info) const
		{
			if (!IfSizeOverlap(cy.position.y, cy.height, movement.y, ci.position.y, 0.0f))
			{
				return false;
			}
			//float normal_y = 1.0f;


			float half_h = cy.height * 0.5f;
			float from_ci_y = cy.position.y - ci.position.y;
			float from_ci_top = from_ci_y + half_h;
			float from_ci_bottom = from_ci_y - half_h;

			float t_y = -1.0f;
			float normal_y = 0.0f;

			// moving up
			if (movement.y > 0.0f)
			{
				// top under circle
				if (from_ci_top <= 0.0f)
				{
					t_y = -from_ci_top / movement.y;
					normal_y = -1.0f; // normal down
				}
			}
			else if (movement.y < 0.0f)
			{
				// bottom above circle
				if (from_ci_bottom >= 0.0f)
				{
					t_y = -from_ci_bottom / movement.y;
					normal_y = 1.0f; // normal up
				}
			}

			// no hit, return
			if (t_y < 0.0f || t_y > 1.0f)
			{
				return false;
			}

			// float t_y = 0.0f;
			// float normal_y = 1.0f;

			// test xz
			Vector3 pos_at_t = cy.position + movement * t_y;
			Vector2 p = { pos_at_t.x, pos_at_t.z };
			auto IsPointInPolygon = [&](const std::vector<Vector2>& vertices) {
				bool inside = false;
				for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
					if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
						(p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
						inside = !inside;
					}
				}
				return inside;
				};

			bool in_outer = IsPointInPolygon(ci.GetOuterVertices(cy.radius));
			bool in_inner = IsPointInPolygon(ci.GetInnerVertices(cy.radius));

			if (in_outer && !in_inner) {
				hit_info.hit = true;
				hit_info.t = t_y;
				hit_info.hit_normal = Vector3(0, normal_y, 0);
				hit_info.hit_position = pos_at_t;

				// 计算 Resolve 后的运动方向（投射到平面上）
				Vector3 movement_canceled = movement * (1.0f - t_y);
				hit_info.movement = movement - hit_info.hit_normal * Vector3::Dot(movement_canceled, hit_info.hit_normal);

				return true;
			}

			return false;

			return false;
		}

		bool operator()(const Cylinder& cy, const CylinderSideInverse& cs, const Vector3& movement, HitInfo& hit_info) const
		{
			float cy_half_h = cy.height * 0.5f;
			float cs_half_h = cs.height * 0.5f;
			// cylinder y range while moving
			float cy_y_min = std::min(cy.position.y, cy.position.y + movement.y) - cy_half_h;
			float cy_y_max = std::max(cy.position.y, cy.position.y + movement.y) + cy_half_h;
			// border y range
			float cs_top = cs.position.y + cs_half_h;
			float cs_bottom = cs.position.y - cs_half_h;
			// y outside, no hits
			if (cy_y_max < cs_bottom || cy_y_min > cs_top)
			{
				return false;
			}

			const float safe_radius = cs.radius - cy.radius;
			if (safe_radius < 0)
			{
				// border smaller than cylinder, no hit(?)
				return false;
			}

			// test xz, check ray-circle hit
			Vector2 movement_xz{ movement.x, movement.z };
			const float movement_xz_dist_sq = movement_xz.LengthSquared();
			if (Math::IsZero(movement_xz_dist_sq))
			{
				// no movement on xz, no hit(?)
				return false;
			}
			const float movement_xz_dist = sqrtf(movement_xz_dist_sq);
			const Vector2 dir_xz{ movement_xz * (1.0f / movement_xz_dist) }; // normalize
			const Vector2 orig_xz = Vector2{ cy.position.x - cs.position.x, cy.position.z - cs.position.z };
			const float dot_orig_dir = Vector2::Dot(dir_xz, orig_xz);
			const float orig_len_sq = orig_xz.LengthSquared();
			float s = dot_orig_dir * dot_orig_dir - orig_len_sq + safe_radius * safe_radius;
			if (Math::IsZero(s))
			{
				s = 0.0f;
			}
			if (s < 0.0f)
			{
				// ray-circle no hit
				return false;
			}
			// get t
			const float s_sqrt = sqrtf(s);
			float t1 = -dot_orig_dir - s_sqrt;
			float t2 = -dot_orig_dir + s_sqrt;

			if (t1 > 0.0f && t2 > 0.0f)
			{
				// already outside? no hit(?)
				return false;
			}

			float t = t2 / movement_xz_dist;
			if (t >= 0.0f && t <= 1.0f)
			{
				// y on hit
				const float y_at_t = cy.position.y + movement.y * t;
				if (y_at_t + cy_half_h < cs_bottom || y_at_t - cy_half_h > cs_top) {
					return false;
				}
				hit_info.hit = true;
				hit_info.t = t;
				// hit point
				Vector3 pos_at_t = cy.position + movement * t;
				Vector3 to_center = (cs.position - pos_at_t);
				to_center.y = 0;
				hit_info.hit_normal = to_center.GetNormalized();
				hit_info.hit_position = pos_at_t;

				// update movement
				//Vector3 movement_canceled = movement * (1.0f - t);
				//Vector3 movement_proj
				//	= movement - hit_info.hit_normal * Vector3::Dot(movement_canceled, hit_info.hit_normal);
				hit_info.movement = movement * (t - 0.01f);// Vector3{ 0.0f };
				hit_info.movement.y = movement.y;

				return true;
			}
			return false;
		}

		bool operator()(const Cylinder& cy, const PolygonCylinder& cp, const Vector3& movement, HitInfo& hit_info) const
		{
			if (cp.sides <= 2)
			{
				return false;
			}
			// TODO: test height
			float cy_half_h = cy.height * 0.5f;
			float cs_half_h = cp.height * 0.5f;
			// cylinder y range while moving
			float cy_y_min = std::min(cy.position.y, cy.position.y + movement.y) - cy_half_h;
			float cy_y_max = std::max(cy.position.y, cy.position.y + movement.y) + cy_half_h;
			// polygon y range
			float cs_top = cp.position.y + cs_half_h;
			float cs_bottom = cp.position.y - cs_half_h;
			// y outside, no hits
			if (cy_y_max < cs_bottom || cy_y_min > cs_top)
			{
				return false;
			}

			// test xz
			const Vector2 ray_start{ cy.position.x, cy.position.z };
			const Vector2 ray_end{ cy.position.x + movement.x, cy.position.z + movement.z };
			HitInfo2D hit_info_xz{};
			for (int i = 0; i < cp.sides; i++)
			{
				//const Vector2 seg_start{ cp.GetPolygonVertexPos(i) };
				//const Vector2 seg_end{ cp.GetPolygonVertexPos(i + 1) };
				//const Vector2 seg_dir{ seg_end - seg_start };
				//const Vector2 seg_expand{ Vector2{ -seg_dir.y, seg_dir.x }.GetNormalized() * cy.radius };
				//// update
				//HitInfo2D hit_info_xz_tmp{};
				//if (IntersectSegments2D(ray_start, ray_end, seg_start + seg_expand, seg_end + seg_expand, hit_info_xz_tmp))
				//{
				//	if (!hit_info_xz.hit || hit_info_xz_tmp.t < hit_info_xz.t)
				//	{
				//		hit_info_xz = hit_info_xz_tmp;
				//	}
				//}
				//if (IntersectSegmentCircle2D(ray_start, ray_end, seg_start, cy.radius, hit_info_xz_tmp))
				//{
				//	if (!hit_info_xz.hit || hit_info_xz_tmp.t < hit_info_xz.t)
				//	{
				//		hit_info_xz = hit_info_xz_tmp;
				//	}
				//}

				// expand to line
				const float cy_angle = float(cp.sides - 2) * Math::HALF_PI / cp.sides;
				const float expand = cy.radius / sinf(cy_angle) - cy.radius; //TDOO
				const Vector2 seg_start{ cp.GetPolygonVertexPosExpanded(i, expand) };
				const Vector2 seg_end{ cp.GetPolygonVertexPosExpanded(i + 1, expand) };
				const Vector2 seg_dir{ seg_end - seg_start };
				// update
				HitInfo2D hit_info_xz_tmp{};
				if (IntersectSegments2D(ray_start, ray_end, seg_start, seg_end, hit_info_xz_tmp))
				{
					if (!hit_info_xz.hit || hit_info_xz_tmp.t < hit_info_xz.t)
					{
						hit_info_xz = hit_info_xz_tmp;
					}
				}
			}
			if (!hit_info_xz.hit)
			{
				return false;
			}
			Vector3 pos_at_t = cy.position + movement * hit_info_xz.t;
			hit_info.hit = true;
			hit_info.t = hit_info_xz.t;
			hit_info.hit_normal = Vector3{ hit_info_xz.hit_normal.x, 0.0f, hit_info_xz.hit_normal.y };
			hit_info.hit_position = pos_at_t;
			hit_info.movement = Vector3{};

			return true;
		}

		bool operator()(const Cylinder& cy_move, const Cylinder& cy_other, const Vector3& movement, HitInfo& hit_info) const
		{
			if (!IfSizeOverlap(cy_move.position.y, cy_move.height, movement.y, cy_other.position.y, cy_other.height))
			{
				return false;
			}

			const float radius_sum = cy_move.radius + cy_other.radius;
			const Vector2 circle_center{ cy_other.position.x, cy_other.position.z };
			const Vector2 ray_start{ cy_move.position.x, cy_move.position.z };
			const Vector2 ray_end{ cy_move.position.x + movement.x,
								   cy_move.position.z + movement.z };
			// test xz
			HitInfo2D hit_info_2d{};
			if (IntersectSegmentCircle2D(ray_start, ray_end, circle_center, radius_sum, hit_info_2d))
			{
				hit_info.hit = true;
				hit_info.t = hit_info_2d.t;
				hit_info.hit_position = cy_move.position + movement * hit_info_2d.t;
				hit_info.hit_normal = Vector3{ hit_info_2d.hit_normal.x, 0.0f, hit_info_2d.hit_normal.y };
				hit_info.movement = Vector3{};
				return true;
			}
			return false;
		}

		bool operator()(const Sphere& sphere, const Plane& plane, const Vector3& movement, HitInfo& hit_info) const
		{
			Vector3 from_plane = sphere.position - plane.position;
			float dist_start = from_plane.Dot(plane.normal);

			// move
			float move_projection = movement.Dot(plane.normal);

			// already overlap / on other side
			if (dist_start < sphere.radius)
			{
				hit_info.hit = true;
				hit_info.t = 0.0f;
				hit_info.hit_normal = plane.normal;
				hit_info.hit_position = sphere.position - plane.normal * dist_start;
				hit_info.penetration = sphere.radius - dist_start;
				return true;
			}

			// moving towards plane
			if (move_projection < Math::EPSILON)
			{
				float t = (sphere.radius - dist_start) / move_projection;
				if (t >= 0.0f && t <= 1.0f)
				{
					hit_info.hit = true;
					hit_info.t = t;
					hit_info.hit_normal = plane.normal;
					hit_info.hit_position = sphere.position + movement * t - plane.normal * sphere.radius;
					return true;
				}
			}

			return false;
		}

		bool operator()(const Sphere& sphere, const PolygonCylinder& cy, const Vector3& movement, HitInfo& hit_info) const
		{
			auto start_info = cy.GetClosestInfo(sphere.position);
			const Vector3 to_start = sphere.position - start_info.closest_point;
			const float dist_to_start = to_start.Length();
			const float dist_to_start_signed = start_info.inside ? -dist_to_start : dist_to_start;
			// inside or overlap at start
			if (dist_to_start_signed < sphere.radius)
			{
				hit_info.hit = true;
				hit_info.t = 0.0f;

				if (Math::IsZero(dist_to_start))
				{
					hit_info.hit_normal = start_info.normal;
				}
				else
				{
					// works for both face and edge
					hit_info.hit_normal = (start_info.inside ? to_start * -1.0f : to_start).GetNormalized();
				}

				hit_info.hit_normal = start_info.normal;

				hit_info.penetration = sphere.radius - dist_to_start_signed;
				hit_info.hit_position = start_info.closest_point;
				return true;
			}

			// move towards
			HitInfo best_hit{};
			// sides
			for (int i = 0; i < cy.sides; ++i)
			{
				Vector2 v0_xz = cy.GetPolygonVertexPos(i);
				Plane side_plane{};
				side_plane.position = Vector3(v0_xz.x, cy.position.y, v0_xz.y);
				side_plane.normal = cy.GetSideNormal(i);

				HitInfo hit_tmp{};
				if (ShapeCollideSwept{}(sphere, side_plane, movement, hit_tmp))
				{
					if (hit_tmp.penetration < 0.0f && (!best_hit.hit || hit_tmp.t < best_hit.t))
					{
						if (cy.IfInSideFace(hit_tmp.hit_position, i))
						{
							best_hit = hit_tmp;
							// flatten normal y
							//best_hit.hit_normal.y = 0.0f;
							//best_hit.hit_normal.Normalize();
						}
					}
				}
			}
			// top and bottom cap
			for (int i = 0; i < 2; ++i)
			{
				Plane cap_plane{};
				cap_plane.normal = i == 0 ? Vector3(0, 1, 0) : Vector3(0, -1, 0);
				cap_plane.position = cy.position;
				cap_plane.position.y = i == 0 ? cy.position.y + cy.height * 0.5f : cy.position.y - cy.height * 0.5f;

				HitInfo hit_tmp{};
				if (ShapeCollideSwept{}(sphere, cap_plane, movement, hit_tmp))
				{
					if (hit_tmp.penetration < 0.0f && (!best_hit.hit || hit_tmp.t < best_hit.t))
					{
						if (cy.IfInYFace(hit_tmp.hit_position))
						{
							best_hit = hit_tmp;
						}
					}
				}
			}

			if (best_hit.hit)
			{
				hit_info = best_hit;
				return true;
			}
			return false;

			//Vector3 target_pos = sphere.position + movement;
			//auto closest_info = cy.GetClosestInfo(target_pos);
			//Vector3 to_sphere_target = target_pos - closest_info.closest_point;

			//if (to_sphere_target.LengthSquared() < sphere.radius * sphere.radius) {

			//	float dist_target = to_sphere_target.Length();

			//	hit_info.hit = true;
			//	hit_info.t = (dist_to_start - sphere.radius) / (dist_to_start - dist_target);
			//	hit_info.hit_normal = to_sphere_target.GetNormalized();
			//	hit_info.hit_position = closest_info.closest_point;
			//	return true;
			//}

			//return false;
		}

		template <typename T, typename U>
		bool operator()(const T&, const U&, const Vector3&, HitInfo&) const {
			return false;
		}
	};


	struct ShapeRayHitXZ {
		bool operator()(const Cylinder& cy, const Ray& ray, float max_distance, HitInfo& hit_info) const
		{
			// y overlap
			if (!IfSizeOverlap(cy.position.y, cy.height, ray.GetOrigin().y, 0.0f))
			{
				return false;
			}
			// test xz
			HitInfo2D hit_info_2d{};
			Vector2 ray_start{ ray.GetOrigin().x, ray.GetOrigin().z };
			Vector2 ray_end{ ray.GetOrigin().x + ray.GetDirection().x * max_distance,
				ray.GetOrigin().z + ray.GetDirection().z * max_distance };
			if (IntersectSegmentCircle2D(ray_start, ray_end,
				Vector2{ cy.position.x, cy.position.z }, cy.radius, hit_info_2d))
			{
				hit_info.hit = true;
				hit_info.t = hit_info_2d.t * max_distance; // t relative to ray
				hit_info.hit_normal = Vector3{ hit_info_2d.hit_normal.x, 0.0f, hit_info_2d.hit_normal.y };
				hit_info.hit_position = ray.At(hit_info.t);
				return true;
			}
			return false;
		}
		bool operator()(const PolygonCylinder& cp, const Ray& ray, float max_distance, HitInfo& hit_info) const
		{
			// y overlap
			if (!IfSizeOverlap(cp.position.y, cp.height, ray.GetOrigin().y, 0.0f))
			{
				return false;
			}
			// test xz
			HitInfo2D hit_info_2d{};
			Vector2 ray_start{ ray.GetOrigin().x, ray.GetOrigin().z };
			Vector2 ray_end{ ray.GetOrigin().x + ray.GetDirection().x * max_distance,
				ray.GetOrigin().z + ray.GetDirection().z * max_distance };
			for (int i = 0; i < cp.sides; i++)
			{
				const Vector2 seg_start{ cp.GetPolygonVertexPos(i) };
				const Vector2 seg_end{ cp.GetPolygonVertexPos(i + 1) };
				HitInfo2D hit_info_2d_tmp{};
				if (IntersectSegments2D(ray_start, ray_end, seg_start, seg_end, hit_info_2d_tmp))
				{
					if (!hit_info_2d.hit || hit_info_2d_tmp.t < hit_info_2d.t)
					{
						hit_info_2d = hit_info_2d_tmp;
					}
				}
			}
			if (hit_info_2d.hit)
			{
				hit_info.hit = true;
				hit_info.t = hit_info_2d.t * max_distance;
				hit_info.hit_normal = Vector3{ hit_info_2d.hit_normal.x, 0.0f, hit_info_2d.hit_normal.y };
				hit_info.hit_position = ray.At(hit_info.t);
				return true;
			}
			return false;
		}
		template <typename T>
		bool operator()(const T&, const Ray&, float, HitInfo&) const
		{
			return false;
		}
	};
}

// overlap with shape
//bool ShapeCollider::IfOverlap(const ShapeCollider& other) const
//{
//    return std::visit([&other](const auto& shape) {
//        return other.IfOverlap(shape);
//        }, m_data);
//}

bool ShapeCollider::IfOverlap(const ShapeCollider& other) const {
	return std::visit(ShapeOverlap{}, this->m_data, other.m_data);
}

bool ShapeCollider::IfCollideSwept(const ShapeCollider& other, const Vector3 movement, HitInfo& hit_info) const
{
	return std::visit([&](const auto& moving_shape, const auto& static_shape) {
		return ShapeCollideSwept{}(moving_shape, static_shape, movement, hit_info);
		}, this->m_data, other.m_data);
}

bool ShapeCollider::IfRayHitXZ(const Ray& ray, float max_distance, HitInfo& hit_info) const
{
	return std::visit([&](const auto& shape) {
		return ShapeRayHitXZ{}(shape, ray, max_distance, hit_info);
		}, this->m_data);
}
// collide with shape
//bool ShapeCollider::IfCollide(const ShapeBase& other, ShapeHitInfo& hit_info) const
//{
//	return std::visit([&other, &hit_info](const auto& shape) {
//		return other.IfCollide(shape, hit_info);
//		}, m_data);
//}
// set transform
void ShapeCollider::SetTransform(TransformNode3D& transform)
{
	std::visit([&transform](auto& shape) {
		shape.SetTransform(transform);
		}, m_data);
}

const Vector3& ShapeCollider::GetPosition() const
{
	return std::visit([](const auto& shape) -> const Vector3& {
		return shape.position;
		}, m_data);
}

//bool ShapeCollider::IfOverlap(const Circle& circle) const
//{
//    return std::visit([&circle](const auto& shape) {
//        return IfOverLap(circle, shape);
//    }, m_data);
//}
//bool ShapeCollider::IfOverlap(const Cylinder& cylinder) const
//{
//    return std::visit([&cylinder](const auto& shape) {
//        return IfOverLap(cylinder, shape);
//    }, m_data);
//}