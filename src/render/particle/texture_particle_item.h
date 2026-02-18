#pragma once
#include "texture_particle.h"
#include "render/config/uv_animation_state.h"
#include "math/transform3d.h"

class TextureParticleItem
{
public:
    void SetConfig(
        const TextureParticleConfig& config,
        const UVFrameAnimationDesc& uv_config,
        const TransformNode3D& transform
    );
    void InitializeParticle();
    void ResetParticle();
    void Update();
    void UpdateCrush();
    void EnterUpdateCrush();
    const TextureParticleConfig& GetConfig() const;
    const UVRect& GetUVRect() const;
    enum class State
    {
        IDLE,
        STATIC,
        CRUSH
	};
	State m_state{ State::IDLE };
    TextureParticleConfig m_particle_config{};
    std::vector<UVFrameAnimationDesc> m_uv_animation_desc{};
    UVAnimationState m_uv_animation_state{};
	TransformNode3D m_transform{};
};