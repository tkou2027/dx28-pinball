#pragma once
#include "render/particle/particle_resource_manager.h"

class ParticleSystem
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	void Update();
	ParticleResourceManager& GetParticleManager() { return m_particle_manager; }
	const ParticleResourceManager& GetParticleManager() const { return m_particle_manager; }
private:
	ParticleResourceManager m_particle_manager{};
};