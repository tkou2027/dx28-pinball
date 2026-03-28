#include "particle_system.h"
#include "global_context.h"
#include "render/render_system.h"

void ParticleSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_particle_manager.Initialize(device, context);
}

void ParticleSystem::Update()
{
	auto& render_swap_context = g_global_context.m_render_system->GetSwapContext();
	auto& particle_swap_data = render_swap_context.GetSwapData().particle_data;
	// update swap data
	// add
	for (const auto& emitter_init_data : particle_swap_data.emitters_to_add)
	{
		m_particle_manager.InitializeEmitter(
			emitter_init_data.desc, emitter_init_data.transform);
	}
	// remove
	for (const auto& emitter_id : particle_swap_data.emitters_to_remove)
	{
		m_particle_manager.RemoveEmitter(emitter_id);
	}
	// update
	m_particle_manager.UpdateEmitters(particle_swap_data.emitters_to_update);

	render_swap_context.ResetParticleSwapData();
}