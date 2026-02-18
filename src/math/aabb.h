#pragma once
// reference: ray tracing in one weekend
#include "math/vector3.h"
#include "math/interval.h"
#include "math/ray.h"

class AABB
{
public:
    static const AABB empty, universe;
    Interval x, y, z;
    AABB() {} // The default AABB is empty, since intervals are empty by default.
    AABB(const Interval& x, const Interval& y, const Interval& z) : x(x), y(y), z(z)
    {
        PadToMinimums();
    }
    AABB(const AABB& a, const AABB& b)
    {
        // x = Interval(std::min(a.x.min, b.x.min), std::max(a.x.max, b.x.max));
        // y = Interval(std::min(a.y.min, b.y.min), std::max(a.y.max, b.y.max));
        // z = Interval(std::min(a.z.min, b.z.min), std::max(a.z.max, b.z.max));
        x = Interval(a.x, b.x);
        y = Interval(a.y, b.y);
        z = Interval(a.z, b.z);
        PadToMinimums();
    }
    AABB(const Vector3& a, const Vector3& b)
    {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.
        x = a[0] < b[0] ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        y = a[1] < b[1] ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        z = a[2] < b[2] ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
        PadToMinimums();
    }
    const Interval& AxisInterval(int axis) const
    {
        switch (axis)
        {
        case 1: { return y; }
        case 2: { return z; }
        default: { return x; }
        }
    }
    int LongestAxis() const
    {
        const float size_x{ x.Size() };
        const float size_y{ y.Size() };
        const float size_z{ z.Size() };
        if (size_x > size_y)
        {
            return size_x > size_z ? 0 : 2;
        }
        return size_y > size_z ? 1 : 2;
    }

    AABB operator+(const Vector3& offset) const
    {
		return AABB(x + offset.x, y + offset.y, z + offset.z);
    }

    // MHOJ Note: this hit function outputs hit interval and no hit record
    // so different from hittable hit,
    // thus not inheriting hittable
    bool hit(
        const Ray& r, Interval& ray_interval) const
    {

        const auto& ray_orig = r.GetOrigin();
        const auto& ray_dir = r.GetDirection();
        bool has_direction = true;
        // for each axis
        for (int axis = 0; axis < 3; ++axis)
        {
            Interval bounding_interval = AxisInterval(axis);

            // deal with dir == 0, ray parallel to certain axis
            // Added by MHOJ
            if (std::abs(ray_dir[axis]) < 1e-160)
            {
                if (ray_orig[axis] >= bounding_interval.m_min && ray_orig[axis] < bounding_interval.m_max)
                {
                    continue;
                }
                return false;
            }
            else
            {
                has_direction = true; // direction is none-zero on at least one direction
            }

            // get ray-interval intersection
            float axis_dir_inv = 1 / ray_dir[axis];
            float t0 = (bounding_interval.m_min - ray_orig[axis]) * axis_dir_inv;
            float t1 = (bounding_interval.m_max - ray_orig[axis]) * axis_dir_inv;

            // make sure t0 < t1
            float ordered = t0 < t1;
            float t_min = ordered ? t0 : t1;
            float t_max = ordered ? t1 : t0;

            // update interval min, max
            ray_interval.m_min = ray_interval.m_min > t_min ? ray_interval.m_min : t_min; // max
            ray_interval.m_max = ray_interval.m_max < t_max ? ray_interval.m_max : t_max; // min
            // no overlap when interval gets empty
            if (ray_interval.m_min >= ray_interval.m_max)
            {
                return false;
            }
        }
        return has_direction;
    }

private:
    void PadToMinimums()
    {
        float delta = Math::EPSILON;
        if (x.Size() < delta)
        {
            x = x.Expand(delta);
        }
        if (y.Size() < delta)
        {
            y = y.Expand(delta);
        }
        if (z.Size() < delta)
        {
            z = z.Expand(delta);
        }
    }
};
