#pragma once
#include <cstdint> // unit32_t

// XorShift32 with help of chatgpt
class Random
{
public:
	uint32_t NextUInt() {
		uint32_t x = m_state;
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		return m_state = x;
	}
	// [min, max] int
	int NextInt(int min, int max) {
		return min + int(NextUInt() % (uint32_t)(max - min + 1));
	}
	// [0, 1) float
	float NextFloat() {
		return (NextUInt() & 0xFFFFFF) / float(0x1000000);
	}
	// [min, max) float
	float NextFloatRange(float min, float max) {
		return min + (max - min) * NextFloat();
	}
private:
	uint32_t m_state{ 2463534242u };
};

extern Random g_random;
