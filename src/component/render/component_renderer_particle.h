#pragma once
#include "component/component.h"
#include "render/particle/particle_desc.h"
#include "render/particle/texture_particle_item.h"
#include "render/config/uv_animation_state.h"
#include "util/countdown_timer.h"

class ComponentRendererParticle : public Component
{
public:
	void InitializeEmitter(const EmitterDesc& desc, const TransformNode3D& transform);
	void InitializeBurst();
	void Update() override;
	void Finalize() override;
	// uv animation
	void SetUVAnimationDesc(const std::vector<UVFrameAnimationDesc>& desc);
	void SetUVAnimationIndex(int index);
	UVAnimationState& GetUVAnimationState() { return m_uv_animation_state; }
	const UVAnimationState& GetUVAnimationState() const { return m_uv_animation_state; }
private:
	void UpdateState();
	void UpdateUVAnimation();
	void ReleaseEmitter();
	EmitterState m_state{ EmitterState::IDLE };

	EmitterDesc m_emitter_desc{};
	TransformNode3D m_transform{};
	// uv animation
	std::vector<UVFrameAnimationDesc> m_uv_frame_animation_desc{};
	UVAnimationState m_uv_animation_state{};
	// timer
	bool m_burst_initialized{ false };
	CountdownTimer m_burst_timer{};
};