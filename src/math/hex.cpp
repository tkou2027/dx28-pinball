#include "hex.h"

HexCoord HexCoord::k_neighbor_offsets[HexCoord::NUM_NEIGHBORS]
{
	HexCoord{1, 0},
	HexCoord{1, -1},
	HexCoord{0, -1},
	HexCoord{-1, 0},
	HexCoord{-1, 1},
	HexCoord{0, 1}
};

std::vector<HexCoord> HexCoord::GenerateRing(int radius, const HexCoord& center)
{
	std::vector<HexCoord> res{};
	if (radius <= 0)
	{
		res.push_back(center);
		return res;
	}

	HexCoord hex = center + k_neighbor_offsets[SPIRAL_INIT_DIR] * radius;
	res.reserve(NUM_NEIGHBORS * static_cast<size_t>(radius));
	for (int i = 0; i < NUM_NEIGHBORS; ++i)
	{
		for (int j = 0; j < radius; ++j)
		{
			res.push_back(hex);
			hex = hex.GetNeighbor(i);
		}
	}
	return res;
}

std::vector<HexCoord> HexCoord::GenerateSpiral(int radius, const HexCoord& center)
{
	std::vector<HexCoord> res{ center }; // always include center
	if (radius <= 0)
	{
		return res;
	}
	res.reserve(static_cast<size_t>(1 + 3 * radius * (radius + 1)));
	for (int k = 1; k <= radius; ++k)
	{
		auto ring = GenerateRing(k, center);
		res.insert(res.end(), ring.begin(), ring.end());
	}
	return res;
}

std::vector<HexCoord> HexCoord::GenerateSpiral(int radius, int radius_inner, const HexCoord& center)
{
	std::vector<HexCoord> res{};
	if (radius_inner >= radius)
	{
		return res; // empty
	}
	if (radius_inner < 0)
	{
		radius_inner = 0;
		res.push_back(center); // include center
	}
	if (radius <= 0)
	{
		return res;
	}
	for (int k = radius_inner + 1; k <= radius; ++k)
	{
		auto ring = GenerateRing(k, center);
		res.insert(res.end(), ring.begin(), ring.end());
	}
	return res;
}
