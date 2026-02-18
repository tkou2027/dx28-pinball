#pragma once
#include "component/component.h"
#include "render/particle/texture_particle_item.h"

class ComponentRendererParticle : public Component
{
public:
	void Update() override { m_item.Update(); }
	TextureParticleItem& GetTextureParticleItem() { return m_item; }
	const TextureParticleItem& GetTextureParticleItem() const { return m_item; }
private:
	TextureParticleItem m_item{};
};