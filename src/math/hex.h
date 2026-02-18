#pragma once
#include <vector>
#include <array>
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

class HexCoord
{
public:
	HexCoord() = default;
	HexCoord(float q_, float r_, float s_) :
		q(q_), r(r_), s(s_) {
	}
	HexCoord(float q_, float r_) :
		q(q_), r(r_), s(0.0f - q_ - r_) {
	}
	HexCoord operator+(const HexCoord& rhs) const
	{
		return HexCoord(q + rhs.q, r + rhs.r, s + rhs.s);
	}
	HexCoord operator*(float t) const
	{
		return HexCoord{ q * t, r * t, s * t };
	}
	HexCoord GetNeighbor(size_t index) const
	{
		assert(index < NUM_NEIGHBORS);
		return (*this) + k_neighbor_offsets[index];
	}
	static std::vector<HexCoord> GenerateRing(int radius, const HexCoord& center = { 0, 0 });
	static std::vector<HexCoord> GenerateSpiral(int radius, const HexCoord& center = { 0, 0 });
	static std::vector<HexCoord> GenerateSpiral(int radius, int radius_inner, const HexCoord& center = { 0, 0 });
public:
	static constexpr size_t NUM_NEIGHBORS{ 6 };
	static constexpr size_t SPIRAL_INIT_DIR{ 4 };
	static HexCoord k_neighbor_offsets[NUM_NEIGHBORS];
	float q{ 0 };
	float r{ 0 };
	float s{ 0 };
};

class Hex
{
public:
	float HexToX(const HexCoord& hex) const
	{
		return std::sqrt(3.0f) * (hex.q + 0.5f * hex.r) * m_size;
	}
	float HexToZ(const HexCoord& hex) const
	{
		return  -(3.0f * 0.5f * hex.r) * m_size;
	}
	void SetSize(float side)
	{
		m_size = side;
	}
	float GetSize() const
	{
		return m_size;
	}
private:
	float m_size{ 1.0f }; // length of one side
};

// reference: catlike coding hex map
struct HexTileMap
{
public:
	void Initialize(float size, const std::vector<HexCoord>& tile_coords)
	{
		m_tile_coords = tile_coords;
		m_hex.SetSize(size);
		const float inner_size = size * std::sqrt(3.0f) * 0.5f;
		const float half_size = size * 0.5f;
		m_corner_offsets[0] = { 0.0f, 0.0f, size };
		m_corner_offsets[1] = { inner_size, 0.0f, half_size };
		m_corner_offsets[2] = { inner_size, 0.0f, -half_size };
		m_corner_offsets[3] = { 0.0f, 0.0f, -size };
		m_corner_offsets[4] = { -inner_size, 0.0f, -half_size };
		m_corner_offsets[5] = { -inner_size, 0.0f, half_size };

		for (const auto& coord : tile_coords)
		{
			const Vector3 center_pos{ m_hex.HexToX(coord), 0.0f, m_hex.HexToZ(coord) };
			AddHexagon(center_pos);
		}
	}
	const std::vector<Vector3>& GetPositions() const
	{
		return m_positions;
	}
	const std::vector<uint32_t>& GetIndices() const
	{
		return m_indices;
	}
	const std::vector<Vector4>& GetColors() const
	{
		return m_pattern_as_color;
	}
private:
	void AddHexagon(const Vector3& center)
	{
		for (int i = 0; i < 6; i++)
		{
			AddTriangle(
				center,
				center + m_corner_offsets[i],
				center + m_corner_offsets[(i + 1) % 6],
				i
			);
		}
	}
	void AddTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, int in_hex_index)
	{
		const uint32_t index = m_positions.size();
		m_positions.push_back(v0);
		m_positions.push_back(v1);
		m_positions.push_back(v2);
		m_indices.push_back(index);
		m_indices.push_back(index + 1);
		m_indices.push_back(index + 2);
		// pattern
		const int pattern = (in_hex_index + 3) % 6 / 2;
		const Vector4 pattern_color{ static_cast<float>(pattern), 0.0f, 0.0f, 0.0f };
		m_pattern_as_color.push_back(pattern_color);
		m_pattern_as_color.push_back(pattern_color);
		m_pattern_as_color.push_back(pattern_color);

	}

	Hex m_hex{};
	std::vector<Vector3> m_positions;
	std::vector<Vector2> texcoords;
	std::vector<Vector4> m_pattern_as_color;
	std::vector<uint32_t> m_indices;
	// constants
	std::array<Vector3, 6> m_corner_offsets;
	std::vector<HexCoord> m_tile_coords;
};
