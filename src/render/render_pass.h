#pragma once

#include <d3d11.h>

#include "render/pass/pass_geometry.h"
#include "render/pass/pass_deferred_shading.h"
#include "render/pass/pass_forward.h"
#include "render/pass/pass_billboard.h"
#include "render/pass/pass_particle.h"

//#include "render/pass/pass_mask.h"
//#include "render/pass/pass_diffuse.h"
//#include "render/pass/pass_outline.h"
#include "render/pass/pass_shadow.h"
#include "render/pass/pass_sky.h"
#include "render/pass/pass_sprite.h"
#include "render/pass/pass_bloom.h"
#include "render/pass/pass_ssr_legacy.h"
#include "render/pass/pass_composite.h"
#include "render/pass/pass_postprocess.h"
#include "render/resource/render_attachment.h"

// class RenderViewBase
// {
// public:
// 	void Initialize(ID3D11Device* device, ID3D11DeviceContext* const);
// 	void Finalize();
// private:
// 	bool active{};
// 	int render_order{};
// 	ViewContext m_context{};
// 
// };
// 
// class RenderProcessBase
// {
// public:
// 	virtual ~RenderProcessBase() = default;
// 	void Initialize(ID3D11Device* device, ID3D11DeviceContext* const)
// 	{
// 	}
// 	void Finalize()
// 	{
// 		for (auto& m_render_views : m_render_views)
// 		{
// 			m_render_views->Finalize();
// 		}
// 	}
// 	void Draw()
// 	{
// 		for (auto& m_render_views : m_render_views)
// 		{
// 			m_render_views->Finalize();
// 		}
// 	}
// private:
// 	std::vector<std::unique_ptr<RenderViewBase> > m_render_views{};
// 	ID3D11Device* m_device{ nullptr };
// 	ID3D11DeviceContext* m_context{ nullptr };
// };

class RenderPass
{
public:
	void Initialize(
		ID3D11Device* pDevice, ID3D11DeviceContext* pContext, IDXGISwapChain* swap_chain);
	void Finalize();
	void UpdateBufferPerFaramePerView();
	void Draw();
	void UpdateViews();
private:
	// TODO: depth prepass
	std::vector<ViewContext> m_view_context;

	// shadow pass
	PassShadow m_pass_shadow{};

	// g-buffer
	PassGeometry m_pass_geometry{};
	// shading
	PassDeferredShading m_pass_deferred_shading{};
	// forward
	PassForward m_pass_forward{};
	// sprites and billboards
	PassBillboard m_pass_billboard{};
	PassParticle m_pass_particle{};
	// sky box
	PassSky m_pass_sky{};

	// post effects


	//PassDiffuse m_pass_diffuse{};
	//PassMask m_pass_mask{};
	// PassOutline m_pass_outline{};

	PassComposite m_pass_composite{};
	PassPostprocess m_pass_post_process{};
	PassBloom m_pass_bloom{};
	PassSprite m_pass_sprite{};
	PassSSRLegacy m_pass_ssr{};

	// render targets
	RenderAttachment m_back_buffer; // back buffer
	RenderAttachment m_g_buffer;

	RenderAttachment m_frame_buffer_dir_light;
	RenderAttachment m_frame_buffer_screen_default;
	RenderAttachment m_frame_buffer_mask;
	RenderAttachment m_frame_buffer_screen_masked;

	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};