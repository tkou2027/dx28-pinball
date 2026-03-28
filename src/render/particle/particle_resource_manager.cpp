#include "particle_resource_manager.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/particle/particle_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/resource/buffer.h"
#include "render/config/model_geometry.h"
#include "texture_particle_item.h"
#include "particle_update_util.h"

using namespace DirectX;

void ParticleResourceManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;

	m_particle_update_util.Initialize(m_device, m_context);
}

void ParticleResourceManager::InitializeEmitter(const EmitterDesc& desc, const DirectX::XMFLOAT4X4& transform)
{
	auto& model_loader = GetModelLoader();

	EmitterData resource{};
	resource.desc = desc;
	resource.transform = transform;

	// index buffer
	//int model_id = config.model_id;
	//if (model_id < 0)

	int model_id{ -1 };
	{
		MeshGeometry mesh_geo{};
		Geometry::CreatePlaneIndex(desc.num_width, desc.num_height, mesh_geo);
		ModelDesc model_desc{};
		model_loader.LoadMeshGeometryUnique(model_desc, mesh_geo);
		resource.model_id = model_desc.model_id;
		// TODO: geometry id
	}
	resource.model_id = model_id;

	// structured buffer
	BufferView buffer_view{};
	UINT num_particles = desc.num_width * desc.num_height;
	Buffer::CreateStructuredBuffer(
		m_device, sizeof(TextureParticleData), num_particles,
		buffer_view.buffer, buffer_view.buffer_srv, buffer_view.buffer_uav
	);
	resource.buffer_particles = buffer_view;

	const auto index = m_emitter_resources.size();
	m_emitter_id_to_index.emplace(resource.desc.id, index);
	m_emitter_resources.push_back(resource);

	// init particle data
	m_particle_update_util.InitializeEmitterParticles(resource);
}

void ParticleResourceManager::RemoveEmitter(ParticleEmitterId emitter_id)
{
	const auto it = m_emitter_id_to_index.find(emitter_id);
	if (it == m_emitter_id_to_index.end())
	{
		assert(false);
		return;
	}
	const size_t index_to_remove = it->second;
	auto& emitter = m_emitter_resources[index_to_remove];
	// release gpu resource
	auto& model_loader = GetModelLoader();
	if (emitter.model_id >= 0)
	{
		// TODO
	}
	emitter.buffer_particles.buffer.Reset();
	emitter.buffer_particles.buffer_srv.Reset();
	emitter.buffer_particles.buffer_uav.Reset();

	// remove from resource list
	const size_t last_index = m_emitter_resources.size() - 1;
	if (index_to_remove != last_index)
	{
		std::swap(m_emitter_resources[index_to_remove], m_emitter_resources[last_index]);
		m_emitter_id_to_index[m_emitter_resources[index_to_remove].desc.id] = index_to_remove;
	}
	m_emitter_resources.pop_back();
	m_emitter_id_to_index.erase(it);
}

void ParticleResourceManager::UpdateEmitters(const std::vector<EmitterUpdateData>& emitters_to_update)
{
	m_active_emitter_ids.clear();
	m_active_emitter_ids.reserve(emitters_to_update.size());
	for (const auto& update_data : emitters_to_update)
	{
		UpdateEmitter(update_data);
		m_active_emitter_ids.push_back(update_data.id);
	}
}

EmitterRenderData ParticleResourceManager::GetEmitterRenderData() const
{
	EmitterRenderData render_data{};
	for (const auto& emitter_id : m_active_emitter_ids)
	{
		const auto it = m_emitter_id_to_index.find(emitter_id);
		if (it == m_emitter_id_to_index.end())
		{
			continue;
		}
		render_data.emitters.push_back(&m_emitter_resources[it->second]);
	}
	return render_data;
}

EmitterData* ParticleResourceManager::GetEmitterResource(ParticleEmitterId emitter_id)
{
	const auto it = m_emitter_id_to_index.find(emitter_id);
	if (it == m_emitter_id_to_index.end())
	{
		return nullptr;
	}
	return &m_emitter_resources[it->second];
}

void ParticleResourceManager::UpdateEmitter(const EmitterUpdateData& update_data)
{
	auto* emitter = GetEmitterResource(update_data.id);
	if (!emitter)
	{
		assert(false);
		return;
	}
	// update desc
	emitter->desc.texture_uv = update_data.texture_uv;
	// update states
	switch (update_data.state)
	{
	case EmitterState::IDLE:
	case EmitterState::INITIALIZED:
	{
		// do nothing
		break;
	}
	case EmitterState::BURST_INIT:
	{
		UpdateEmitterBurstInit(update_data, *emitter);
		break;
	}
	case EmitterState::BURST_UPDATE:
	{
		UpdateEmitterBurstUpdate(update_data, *emitter);
		break;
	}
	}
}

void ParticleResourceManager::UpdateEmitterBurstInit(const EmitterUpdateData& update_data, EmitterData& emitter)
{

}

void ParticleResourceManager::UpdateEmitterBurstUpdate(const EmitterUpdateData& update_data, EmitterData& emitter)
{
	m_particle_update_util.UpdateEmitterBurstUpdate(update_data, emitter);
}

ModelLoader& ParticleResourceManager::GetModelLoader() const
{
	return g_global_context.m_render_system->GetRenderResource().GetModelLoader();
}
