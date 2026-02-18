#include "particle_system.h"

void ParticleSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_texture_particle_manager.Initialize(device, context);
}