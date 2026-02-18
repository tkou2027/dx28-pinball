#pragma once

#include <array>
#include <unordered_map>
#include "render/render_path.h"
// passes
#include "render/pass/pass_geometry.h"
#include "render/pass/pass_deferred_shading.h"
#include "render/pass/pass_postprocess.h"
#include "render/pass/pass_particle.h"
#include "render/pass/pass_forward.h"
#include "render/pass/pass_sky.h"

#include "render/pass/pass_bloom.h"
#include "render/pass/pass_composite.h"

#include "render/util/gpu_timer.h"

class RenderPathScreenReflect : public RenderPathBase
{
public:
	void Initialize() override;
	void InitializeViewContext(RenderViewKey view_id, uint32_t width, uint32_t height) override;
	void UpdateVisibleRenderables(
		const class SceneRenderablesManager& renderables,
		CameraRenderLayer render_layer
	);
	void Draw(RenderViewKey view_key, const ViewContext& view_context) override;
	void Finalize() override;
private:
	// textures
	struct InternalTextures
	{
		TextureResource2D frame_buffer_color{};
		TextureResource2D emission{};
		TextureResource2D frame_buffer_depth{};
	};
	struct InternalRenderTargets
	{
		RenderTarget g_buffer{};
		RenderTarget frame_buffer{};
	};

	void BuildRenderTargets(const InternalTextures& textures, uint32_t width, uint32_t height);

	// forward
	PassForward m_pass_forward{};
	// post process
	PassPostprocess m_pass_post_process{};

	// camera / variation id => textures
	std::unordered_map<RenderViewKey, InternalTextures> m_texture_resources;
	InternalRenderTargets m_render_targets{};

	static constexpr size_t GPU_TIMER_COUNT{ 3 };
	std::array< GpuTimer, GPU_TIMER_COUNT> m_gpu_timers{};
};