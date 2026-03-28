#include "texture_particle_item.h"
#include "global_context.h"
#include "platform/timer.h"
#include "render/render_system.h"
#include "render/particle/particle_system.h"

void ParticleEmitter::SetConfig(
	const EmitterDesc& config,
	const UVFrameAnimationDesc& uv_config,
	const TransformNode3D& transform)
{
	m_particle_config = config;
	m_uv_animation_desc.push_back(uv_config);
	m_uv_animation_state.SetAnimationIndex(0, m_uv_animation_desc);
	m_transform = transform;
}

void ParticleEmitter::InitializeParticle()
{
	//assert(m_particle_config.buffer_view_id == -1);
	//auto& manager = g_global_context.m_render_system->GetParticleSystem().GetParticleManager();
	//m_particle_config.buffer_view_id = manager.InitializeOrResetParticle(
	//	m_particle_config, m_transform.GetFloat4x4Global());
	m_state = State::STATIC;
}

void ParticleEmitter::ResetParticle()
{
	//assert(m_particle_config.buffer_view_id != -1);
	//auto& manager = g_global_context.m_render_system->GetParticleSystem().GetParticleManager();
	//m_particle_config.buffer_view_id = manager.InitializeOrResetParticle(
	//	m_particle_config, m_transform.GetFloat4x4Global());
	m_state = State::STATIC;
}

void ParticleEmitter::Update()
{
	switch (m_state)
	{
	case State::STATIC:
	{
		m_uv_animation_state.Update(m_uv_animation_desc);
		break;
	}
	case State::CRUSH:
	{
		UpdateCrush();
		break;
	}
	}
	// TODO: update destroy
}

void ParticleEmitter::UpdateCrush()
{
	//auto& manager = g_global_context.m_render_system->GetParticleSystem().GetParticleManager();
	//manager.UpdateParticles(m_particle_config, g_global_context.m_timer->GetDeltaTime());
}

void ParticleEmitter::EnterUpdateCrush()
{
	if (m_state != State::STATIC)
	{
		return;
	}

	m_state = State::CRUSH;
}

const EmitterDesc& ParticleEmitter::GetConfig() const
{
	return m_particle_config;
}
