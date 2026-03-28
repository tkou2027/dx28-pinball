#pragma once
#include "particle_resource_manager.h"
#include "render/config/uv_animation_state.h"
#include "math/transform3d.h"

class ParticleEmitter
{
public:
    void SetConfig(
        const EmitterDesc& config,
        const UVFrameAnimationDesc& uv_config,
        const TransformNode3D& transform
    );
	const ParticleEmitterId& GetId() const { return m_particle_config.id; }
    void InitializeParticle();
    void ResetParticle();
    void Update();
    void UpdateCrush();
    void EnterUpdateCrush();
    const EmitterDesc& GetConfig() const;
    const UVRect& GetUVRect() const;
    enum class State
    {
        IDLE,
        STATIC,
        CRUSH
	};
	State m_state{ State::IDLE };
    EmitterDesc m_particle_config{};
    std::vector<UVFrameAnimationDesc> m_uv_animation_desc{};
    UVAnimationState m_uv_animation_state{};
	TransformNode3D m_transform{};
};