#pragma once
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include "render/directx.h"
#include "render/resource/shader.h"
#include "render/config/texture_resource_id.h"
#include "render/particle/particle_desc.h"
#include "render/particle/particle_data.h"
#include "render/particle/particle_update_util.h"

struct EmitterRenderData
{
	std::vector<const EmitterData*> emitters;
};

class ParticleResourceManager
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	void InitializeEmitter(const EmitterDesc& desc, const DirectX::XMFLOAT4X4& transform);
	void RemoveEmitter(ParticleEmitterId emitter_id);
	void UpdateEmitters(const std::vector<EmitterUpdateData>& emitters_to_update);
	EmitterRenderData GetEmitterRenderData() const;
private:
	EmitterData* GetEmitterResource(ParticleEmitterId emitter_id);
	void UpdateEmitter(const EmitterUpdateData& update_data);
	void UpdateEmitterBurstInit(const EmitterUpdateData& update_data, EmitterData& emitter);
	void UpdateEmitterBurstUpdate(const EmitterUpdateData& update_data, EmitterData& emitter);
	class ModelLoader& GetModelLoader() const;

	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
	// utils for executing compute shaders
	ParticleUpdateUtil m_particle_update_util;
	// resources
	std::unordered_map<ParticleEmitterId, size_t> m_emitter_id_to_index;
	std::vector<ParticleEmitterId > m_active_emitter_ids;
	std::vector<EmitterData> m_emitter_resources;
};