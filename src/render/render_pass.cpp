#include "render_pass.h"

#include "global_context.h"
#include "render/render_system.h"
#include "config/constant.h"

void RenderPass::Initialize(
	ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swap_chain)
{
	m_device = device;
	m_context = context;

	// shadow
	m_pass_shadow.Initialize(device, context);
	// deferred
	m_pass_geometry.Initialize(device, context);
	m_pass_deferred_shading.Initialize(device, context);
	// forward
	m_pass_forward.Initialize(device, context);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardUnlit>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardScreen>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardGlass>(), PassForward::SubPassQueue::DEFAULT);

	m_pass_billboard.Initialize(device, context);
	m_pass_particle.Initialize(device, context);

	// post process
	m_pass_bloom.Initialize(device, context);
	m_pass_ssr.Initialize(device, context);
	m_pass_composite.Initialize(device, context);

	//m_pass_diffuse.Initialize(device, context);
	//m_pass_mask.Initialize(device, context);
	//m_pass_outline.Initialize(device, context);

	m_pass_sky.Initialize(device, context);
	m_pass_sprite.Initialize(device, context);
	m_pass_post_process.Initialize(device, context);

	// screen
	m_back_buffer.Initialize(device, context, SCREEN_WIDTH, SCREEN_HEIGHT);
	m_back_buffer.AddTextureColorFromSwapChain(swap_chain);

	// g-buffer
	m_g_buffer.Initialize(device, context, SCREEN_WIDTH, SCREEN_HEIGHT);
	//m_g_buffer.AddTextureColor(TextureResource2D::GetTextureDesc(
	//	DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
	//)); // buffer_a, world normal, TODO
	m_g_buffer.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET); // buffer_a, world normal, TODO
	m_g_buffer.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET); // buffer_b, material
	m_g_buffer.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET); // buffer_c, base color
	m_g_buffer.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET_HDR); // buffer_d, emission
	m_g_buffer.SetTextureDepthStencil(TextureResource2D::DESC_PREST_DEPTH_STENCIL);

	// frame buffer
	// rtt
	m_frame_buffer_screen_default.Initialize(device, context, SCREEN_WIDTH, SCREEN_HEIGHT);
	// m_frame_buffer_screen_default.AddTextureColor(TextureResource,2D::DESC_PREST_RENDER_TARGET_HDR);
	m_frame_buffer_screen_default.AddTextureColor(m_g_buffer.GetTextureColor<TextureResource2D>(3));
	m_frame_buffer_screen_default.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET_HDR);
	m_frame_buffer_screen_default.SetTextureDepthStencil(TextureResource2D::DESC_PREST_DEPTH_STENCIL);
	//m_frame_buffer_screen_default.SetTextureDepthStencil(m_g_buffer.GetTextureDepthStencil());

	//// frame buffer
	//// mask
	//m_frame_buffer_mask.Initialize(device, context, SCREEN_WIDTH, SCREEN_HEIGHT);
	//m_frame_buffer_mask.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET);

	//// us,ing the same depth buffer...
	//m_frame_buffer_mask.SetTextureDepthStencil(m_frame_buffer_screen_default.GetTextureDepthStencil());

	//// shadow
	m_frame_buffer_dir_light.Initialize(device, context, 2048, 2048);
	m_frame_buffer_dir_light.SetTextureDepthStencil(TextureResource2D::DESC_PREST_DEPTH_STENCIL);

	// share textures
	m_pass_deferred_shading.SetGBuffer(m_g_buffer);
	m_pass_deferred_shading.SetTextureShadow(m_frame_buffer_dir_light.GetTextureDepthStencil()->GetShaderResourceView().Get());


	//m_pass_diffuse.SetDirLightTexture(m_frame_buffer_dir_light.GetTextureDepthStencil()->GetShaderResourceView().Get());
	m_pass_bloom.SetInputTextureAdd(
		m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(0)->GetShaderResourceView().Get());
	m_pass_bloom.SetInputTextureBlur(
		m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(1)->GetShaderResourceView().Get());

	
	m_pass_ssr.SetGBuffer(m_g_buffer);
	// m_pass_ssr.SetColorTexture(m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(0)->GetShaderResourceView().Get());
	m_pass_ssr.SetColorTexture(m_pass_bloom.GetRt());

	//m_pass_composite.SetInputTextureBase(m_pass_bloom.GetRt());
	//m_pass_composite.SetInputTextureReflection(m_pass_ssr.GetRt());

	m_pass_post_process.SetInputResource(m_pass_ssr.GetRt());
	//m_pass_ssr.SetInputTexturePosition(
	//	m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(2)->GetShaderResourceView().Get());
	//m_pass_ssr.SetInputTextureNormal(
	//	m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(3)->GetShaderResourceView().Get());
	//m_pass_ssr.SetInputTextureInput(
	//	m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(0)->GetShaderResourceView().Get());


	m_pass_post_process.SetInputResource(
		m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(0)->GetShaderResourceView().Get());
}

void RenderPass::Finalize()
{
	m_pass_post_process.Finalize();
	m_pass_sprite.Finalize();
	m_pass_sky.Finalize();
	m_pass_shadow.Finalize();
}

void RenderPass::Draw()
{
	// auto& render_resource = g_global_context.m_render_system->GetRenderResource();
	// shadow mapping
	// m_pass_post_process.Draw();
	m_frame_buffer_dir_light.ClearDepthStencil(D3D11_CLEAR_DEPTH);
	m_frame_buffer_dir_light.Bind();
	m_pass_shadow.Draw();

	float clear_color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
	float clear_color_all[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	m_g_buffer.ClearColor(clear_color_all);
	m_g_buffer.ClearDepthStencil(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	m_g_buffer.Bind();
	m_pass_geometry.Draw();

	// m_frame_buffer_screen_default.ClearColor(clear_color);
	// copy emission to bloom buffer
	m_context->CopyResource(
		m_frame_buffer_screen_default.GetTextureColor<TextureResource2D>(1)->GetTexture().Get(),
		m_g_buffer.GetTextureColor<TextureResource2D>(3)->GetTexture().Get()
	);
	m_context->CopyResource(
		m_frame_buffer_screen_default.GetTextureDepthStencil()->GetTexture().Get(),
		m_g_buffer.GetTextureDepthStencil()->GetTexture().Get()
	);
	// m_frame_buffer_screen_default.ClearDepthStencil(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	m_frame_buffer_screen_default.Bind();

	// skybox
	// deferred shading
	m_pass_deferred_shading.Draw();
	// forward objects
	m_pass_forward.SetDrawingQueue(PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.Draw();
	m_pass_sky.Draw(); // TODO use g buffer depth
	
	m_pass_billboard.Draw();
	m_pass_particle.Draw();

	//m_pass_outline.Draw();
	//m_pass_diffuse.Draw();

	//m_frame_buffer_mask.ClearColor(clear_color);
	//m_frame_buffer_mask.Bind();
	//m_pass_mask.Draw();

	//m_pass_diffuse.SetUseMask(true);
	//m_frame_buffer_screen_default.Bind();
	//m_pass_diffuse.Draw();
	//m_pass_diffuse.SetUseMask(false);


	//m_pass_sky.SetUseMask(true);
	//m_pass_sky.Draw();
	//m_pass_sky.SetUseMask(false);

	//m_pass_mask.DrawWireframe();

	m_pass_bloom.Draw();
	m_pass_bloom.DrawOutput();
	m_pass_ssr.Draw();

	m_back_buffer.ClearColor(clear_color);
	m_back_buffer.Bind();

	// postprocess
	// m_pass_post_process.Draw();
	m_pass_composite.Draw();

	// UI
	m_pass_sprite.Draw();
}
