#pragma once
#include "render/directx.h"
#include "render/particle/texture_particle.h"

class ParticleSystem
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	void Update();
	TextureParticleManager& GetTextureParticleManager() { return m_texture_particle_manager;  }
private:
	TextureParticleManager m_texture_particle_manager{};
};