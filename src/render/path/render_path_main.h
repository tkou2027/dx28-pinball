#pragma once

#include <array>
#include <unordered_map>
#include "render/render_path.h"
// passes
// #include "render/pass/pass_depth.h"
#include "render/pass/prepass/pass_depth_normal.h"
#include "render/pass/pass_geometry.h"
#include "render/pass/pass_deferred_shading.h"
#include "render/pass/pass_postprocess.h"
#include "render/pass/pass_particle.h"
#include "render/pass/pass_forward.h"
#include "render/pass/pass_sky.h"

#include "render/pass/pass_bloom.h"
//#include "render/pass/pass_ssr.h"
#include "render/pass/pass_composite.h"
#include "render/pass/postprocess/pass_ssr.h"
#include "render/pass/postprocess/pass_blur_dual.h"

// sprite
#include "render/pass/pass_sprite.h"

#include "render/util/gpu_timer.h"

class RenderPathMain : public RenderPathBase
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
	static constexpr int POST_PROCESS_TEMP_COUNT{ 2 };
	struct InternalTextures
	{
		// g-buffer
		TextureResource2D g_buffer_a{};
		TextureResource2D g_buffer_b{};
		TextureResource2D g_buffer_c{};
		TextureResource2D emission{};
		TextureResource2D g_buffer_depth{};
		// frame
		TextureResource2D frame_buffer_color{};
		TextureResource2D frame_buffer_depth{};
		// post process
		PassBlurDual::InternalTextures blur_textures{};
		PassSSR::InternalTextures ssr_textures{};
		TextureResource2D post_process_temps[POST_PROCESS_TEMP_COUNT]{};
	};
	struct InternalRenderTargets
	{
		RenderTarget pre_pass{};
		RenderTarget g_buffer{};
		RenderTarget deferred_shading{};
		RenderTarget frame_buffer{};
		RenderTarget post_process_temps[POST_PROCESS_TEMP_COUNT]{};
	};

	void BuildRenderTargets(const InternalTextures& textures, uint32_t width, uint32_t height);

	// depth pre-pass
	PassDepthNormal m_pass_depth_normal{};
	// g-buffer
	PassGeometry m_pass_geometry{};
	// shading
	PassDeferredShading m_pass_deferred_shading{};
	// forward
	PassForward m_pass_forward{};
	class SubPassForwardProjector* m_pass_projector{};
	// sepecial objects
	PassParticle m_pass_particle{};
	PassSky m_pass_sky{};
	// post process
	PassPostprocess m_pass_post_process{};
	PassComposite m_pass_composite{};
	PassBloom m_pass_bloom{};
	// PassSSRLegacy m_pass_ssr{};
	PassSSR m_pass_ssr{};
	PassBlurDual m_pass_blur_dual{};

	PassSprite m_pass_sprite{};

	// camera / variation id => textures
	std::unordered_map<RenderViewKey, InternalTextures> m_texture_resources;
	InternalRenderTargets m_render_targets{};

	static constexpr size_t GPU_TIMER_COUNT{ 3 };
	std::array< GpuTimer, GPU_TIMER_COUNT> m_gpu_timers{};
};