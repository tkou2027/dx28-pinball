#pragma once
#include "math.h"

class Interval
{
public:
	static const float infinity;
	static const Interval empty, universe;
	float m_min, m_max;
	Interval() : m_min(+infinity), m_max(-infinity) {};
	Interval(float min, float max) : m_min(min), m_max(max) {};
	// MHOJ Note: join two intervals
	Interval(const Interval& a, const Interval& b)
	{
		// Create the interval tightly enclosing the two input intervals.
		m_min = a.m_min <= b.m_min ? a.m_min : b.m_min;
		m_max = a.m_max >= b.m_max ? a.m_max : b.m_max;
	}
	float Size() const
	{
		return m_max - m_min;
	}
	bool Contains(float x) const
	{
		return x >= m_min && x <= m_max;
	}
	bool Surrounds(float x) const
	{
		return x > m_min && x < m_max;
	}
	float Clamp(float x) const
	{
		return x < m_min ? m_min : (x > m_max ? m_max : x);
	}
	Interval Expand(float delta) const
	{
		auto padding = delta / 2;
		return Interval(m_min - padding, m_max + padding);
	}
	Interval operator+(float displacement) const
	{
		return Interval(m_min + displacement, m_max + displacement);
	}
};