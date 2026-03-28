#include "component_renderer_particle.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/particle/particle_emitter_id.h"
#include "platform/timer.h"

void ComponentRendererParticle::InitializeEmitter(const EmitterDesc& desc, const TransformNode3D& transform)
{
	assert(m_state == EmitterState::IDLE);

	m_emitter_desc = desc;
	m_emitter_desc.id = ParticleEmitterIdAllocator::Allocate();
	m_transform = transform;

	// init resource
	EmitterInitData emitter_init_data{};
	emitter_init_data.desc = m_emitter_desc;
	emitter_init_data.transform = m_transform.GetFloat4x4Global();
	auto& render_swap_data = g_global_context.m_render_system->GetSwapContext().GetSwapData();
	render_swap_data.particle_data.emitters_to_add.push_back(emitter_init_data);

	m_state = EmitterState::INITIALIZED;
}

void ComponentRendererParticle::InitializeBurst()
{
	assert(m_state != EmitterState::IDLE);
	m_state = EmitterState::BURST_INIT;
	m_burst_initialized = false;
}

void ComponentRendererParticle::Update()
{
	UpdateState(); // state transition
	UpdateUVAnimation(); // uv animation update
	if (m_state == EmitterState::IDLE)
	{
		return;
	}
	// update to swap data
	EmitterUpdateData emitter_update_data{};
	emitter_update_data.id = m_emitter_desc.id;
	emitter_update_data.state = m_state;
	emitter_update_data.delta_time = g_global_context.m_timer->GetDeltaTime();
	emitter_update_data.texture_uv = m_uv_animation_state.GetUVRect(m_uv_frame_animation_desc);
	// emitter_update_data.transform = m_transform.GetFloat4x4Global();
	auto& render_swap_data = g_global_context.m_render_system->GetSwapContext().GetSwapData();
	render_swap_data.particle_data.emitters_to_update.push_back(emitter_update_data);
}

void ComponentRendererParticle::Finalize()
{
	ReleaseEmitter();
}

void ComponentRendererParticle::SetUVAnimationDesc(const std::vector<UVFrameAnimationDesc>& desc)
{
	m_uv_frame_animation_desc = desc;
}

void ComponentRendererParticle::SetUVAnimationIndex(int index)
{
	m_uv_animation_state.SetAnimationIndex(index, m_uv_frame_animation_desc);
}

void ComponentRendererParticle::UpdateState()
{
	switch (m_state)
	{
	case EmitterState::IDLE:
	case EmitterState::INITIALIZED:
	{
		break;
	}
	case EmitterState::BURST_INIT:
	{
		if (m_burst_initialized)
		{
			// burst update
			m_burst_timer.Initialize(5.0f); // TODO
			m_state = EmitterState::BURST_UPDATE;
		}
		m_burst_initialized = true;
		break;
	}
	case EmitterState::BURST_UPDATE:
	{
		float t = m_burst_timer.Update(g_global_context.m_timer->GetDeltaTime());
		if (t <= 0.0f)
		{
			ReleaseEmitter();
		}
		break;
	}
}
}

void ComponentRendererParticle::UpdateUVAnimation()
{
	switch (m_state)
	{
	case EmitterState::IDLE:
	{
		break;
	}
	default:
	{
		m_uv_animation_state.Update(m_uv_frame_animation_desc);
		break;
	}
	}
}

void ComponentRendererParticle::ReleaseEmitter()
{
	if (m_state == EmitterState::IDLE)
	{
		return;
	}
	auto& render_swap_data = g_global_context.m_render_system->GetSwapContext().GetSwapData();
	render_swap_data.particle_data.emitters_to_remove.push_back(m_emitter_desc.id);
	m_state = EmitterState::IDLE;
}
