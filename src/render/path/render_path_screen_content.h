#pragma once

#include <array>
#include <unordered_map>
#include "render/render_path.h"
// passes
#include "render/pass/pass_forward.h"
#include "render/pass/pass_sprite.h"

#include "render/util/gpu_timer.h"

class RenderPathScreenContent : public RenderPathBase
{
public:
	void Initialize() override;
	void InitializeViewContext(RenderViewKey view_id, uint32_t width, uint32_t height) override;
	void UpdateVisibleRenderables(
		const class SceneRenderablesManager& renderables,
		CameraRenderLayer render_layer
	) override;
	void Draw(RenderViewKey view_key, const ViewContext& view_context) override;
	void Finalize() override;
private:
	PassForward m_pass_forward{};
	PassSprite m_pass_sprite{};

	static constexpr size_t GPU_TIMER_COUNT{ 1 };
	std::array< GpuTimer, GPU_TIMER_COUNT> m_gpu_timers{};
};