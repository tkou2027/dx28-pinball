#pragma once

// this is named "swap data" because i was inspired by
// some other engine(, where swap data helps to handle
// logic and rendering in different threads.
// but i did't actually implement threading here yet.
// swap data is just a structure to hold
// rendering-related data to be updated each frame.

#include <vector>
#include "render/config/camera_data.h"
#include "render/particle/particle_desc.h"

// camera
struct CameraUpdateData
{
	CameraUsageConfig config{};
	CameraShapeConfig shape{};
	CameraTransformData transform{};
	// TODO: resize
};

struct CameraSwapData
{
	std::vector<CameraUsageConfig> cameras_to_add{};
	std::vector<CameraUpdateData> cameras_to_update{};
	std::vector<CameraUsageConfig> cameras_to_remove{};
};

// particle
struct ParticleSwapData
{
	std::vector<EmitterInitData> emitters_to_add{};
	std::vector<EmitterUpdateData> emitters_to_update{};
	std::vector<ParticleEmitterId> emitters_to_remove{};
};

struct RenderSwapData
{
	CameraSwapData camera_data{};
	ParticleSwapData particle_data{};
};

class RenderSwapContext
{
public:
	RenderSwapData& GetSwapData() { return m_swap_data; }
	void ResetCameraSwapData()
	{
		m_swap_data.camera_data.cameras_to_add.clear();
		m_swap_data.camera_data.cameras_to_update.clear();
		m_swap_data.camera_data.cameras_to_remove.clear();
	}
	void ResetParticleSwapData()
	{
		m_swap_data.particle_data.emitters_to_add.clear();
		m_swap_data.particle_data.emitters_to_update.clear();
		m_swap_data.particle_data.emitters_to_remove.clear();
	}
private:
	RenderSwapData m_swap_data{}; // only one swap data for now
};