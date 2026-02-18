#include "render/path/render_path_main.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include <chrono>
#include "util/debug_ostream.h"
#include "render/pass/geometry/subpass_geometry_floor.h"
#include "render/pass/geometry/subpass_geometry_cel.h"
#include "render/pass/forward/subpass_forward_projector.h"
#include "render/pass/forward/subpass_forward_silhouette.h"

void RenderPathMain::Initialize()
{
	ID3D11Device* device = GetDevice();
	ID3D11DeviceContext* context = GetDeviceContext();

	// initialize passes
	// depth pre-pass
	m_pass_depth_normal.Initialize(device, context);
	// geometry
	m_pass_geometry.Initialize(device, context);
	m_pass_geometry.AddSubPass(std::make_unique<PassGeometryFloor>());
	m_pass_geometry.AddSubPass(std::make_unique<PassGeometryCel>());
	// deferred shading
	m_pass_deferred_shading.Initialize(device, context);
	// forward
	m_pass_forward.Initialize(device, context);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardUnlit>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardSilhouette>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardScreen>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardGlass>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardProjector>(), PassForward::SubPassQueue::DECAL);
	m_pass_projector = &m_pass_forward.GetSubPass<SubPassForwardProjector>(PassForward::SubPassQueue::DECAL);
	// special objects
	m_pass_particle.Initialize(device, context);
	m_pass_sky.Initialize(device, context);
	m_pass_post_process.Initialize(device, context);
	// post process
	// m_pass_bloom.Initialize(device, context);
	m_pass_blur_dual.Initialize(device, context);
	m_pass_ssr.Initialize(device, context);
	m_pass_composite.Initialize(device, context);
	// sprites
	m_pass_sprite.Initialize(device, context);

	for (auto& timer : m_gpu_timers)
	{
		timer.Initialize(device, context);
	}
}

void RenderPathMain::InitializeViewContext(RenderViewKey view_key, uint32_t width, uint32_t height)
{
	if (m_texture_resources.find(view_key) != m_texture_resources.end())
	{
		// already initialized
		// TODO: resize
		return;
	}
	// common texture desc
	const auto render_target_desc = TextureResource::BuildTexturesDesc(
		TextureResource2D::DESC_PREST_RENDER_TARGET, width, height);
	const auto render_target_hdr_desc = TextureResource::BuildTexturesDesc(
		TextureResource2D::DESC_PREST_RENDER_TARGET_HDR, width, height);
	const auto depth_stencil_desc = TextureResource::BuildTexturesDesc(
		TextureResource2D::DESC_PREST_DEPTH_STENCIL, width, height);

	// initialize textures
	ID3D11Device* device = GetDevice();
	InternalTextures textures{};
	// g-buffer
	textures.g_buffer_a.InitializeRenderTarget2D(device, render_target_desc);
	textures.g_buffer_b.InitializeRenderTarget2D(device, render_target_desc);
	textures.g_buffer_c.InitializeRenderTarget2D(device, render_target_desc);
	textures.g_buffer_a.InitializeRenderTarget2D(device, render_target_desc);
	textures.emission.InitializeRenderTarget2D(device, render_target_hdr_desc);
	textures.g_buffer_depth.InitializeDepth2D(device, depth_stencil_desc);

	// frame
	textures.frame_buffer_color.InitializeRenderTarget2D(device, render_target_hdr_desc);
	textures.frame_buffer_depth.InitializeDepth2D(device, depth_stencil_desc);
	// post process
	m_pass_blur_dual.CreateTextures(width, height, textures.blur_textures);
	m_pass_ssr.CreateTextures(width, height, textures.ssr_textures);
	textures.post_process_temps[0].InitializeRenderTarget2D(device, render_target_hdr_desc);
	textures.post_process_temps[1].InitializeRenderTarget2D(device, render_target_hdr_desc);

	m_texture_resources.emplace(view_key, textures);
}

void RenderPathMain::UpdateVisibleRenderables(
	const SceneRenderablesManager& scene_renderables, CameraRenderLayer render_layer)
{
	auto start = std::chrono::high_resolution_clock::now();

	const auto& material_resource = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	const auto& visible_info = scene_renderables.GetRenderablesOfLayer(render_layer);

	m_pass_depth_normal.ResetRenderableIndices(render_layer);
	m_pass_geometry.ResetRenderableIndices(render_layer);
	m_pass_sky.ResetRenderableIndices(render_layer);
	m_pass_forward.ResetRenderableIndices(render_layer);
	m_pass_sprite.SetRenderLayer(render_layer);

	// models
	for (const auto& index : visible_info.indices_model)
	{
		const auto& model_info = scene_renderables.m_models[index];
		const auto& material = material_resource.GetMaterialDesc(model_info.key.material_id);
		m_pass_depth_normal.AddRenderableIndex(index, model_info.key.model_type, material);
		m_pass_geometry.AddRenderableIndex(index, model_info.key.model_type, material);
		m_pass_forward.AddRenderableIndex(index, model_info.key.model_type, material);
		m_pass_sky.AddRenderableIndex(index, model_info.key.model_type, material);
	}

	auto finish = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "updating view" << duration.count() << " " << std::endl;
}

void RenderPathMain::BuildRenderTargets(const InternalTextures& textures, uint32_t width, uint32_t height)
{
	// "render targets" are reall just a collection of RTVs and DSVs (and view port) bound together
	// to simplify binding and clearing code.
	m_render_targets.pre_pass.Reset(width, height);
	m_render_targets.pre_pass.SetDepthStencil(textures.g_buffer_depth.GetDepthStencilView());

	m_render_targets.g_buffer.Reset(width, height);
	m_render_targets.g_buffer.AddRenderTarget(textures.g_buffer_a.GetRenderTargetView());
	m_render_targets.g_buffer.AddRenderTarget(textures.g_buffer_b.GetRenderTargetView());
	m_render_targets.g_buffer.AddRenderTarget(textures.g_buffer_c.GetRenderTargetView());
	m_render_targets.g_buffer.AddRenderTarget(textures.emission.GetRenderTargetView());
	m_render_targets.g_buffer.SetDepthStencil(textures.g_buffer_depth.GetDepthStencilView());

	m_render_targets.deferred_shading.Reset(width, height);
	// same as frame buffer color
	m_render_targets.deferred_shading.AddRenderTarget(textures.frame_buffer_color.GetRenderTargetView());
	// no depth stencil, for deferred shading needs to read g-buffer depth

	m_render_targets.frame_buffer.Reset(width, height);
	m_render_targets.frame_buffer.AddRenderTarget(textures.frame_buffer_color.GetRenderTargetView());
	m_render_targets.frame_buffer.AddRenderTarget(textures.emission.GetRenderTargetView());
	//m_render_targets.frame_buffer.SetDepthStencil(textures.frame_buffer_depth.GetDepthStencilView());
	// use g-buffer depth for forward rendering, avoid copying
	m_render_targets.frame_buffer.SetDepthStencil(textures.g_buffer_depth.GetDepthStencilView());


	for (int i = 0; i < POST_PROCESS_TEMP_COUNT; ++i)
	{
		m_render_targets.post_process_temps[i].Reset(width, height);
		m_render_targets.post_process_temps[i].AddRenderTarget(textures.post_process_temps[i].GetRenderTargetView());
	}

	// initialize resource inputs
	m_pass_deferred_shading.SetInputResource(
		textures.g_buffer_a.GetShaderResourceView().Get(),
		textures.g_buffer_b.GetShaderResourceView().Get(),
		textures.g_buffer_c.GetShaderResourceView().Get(),
		textures.g_buffer_depth.GetShaderResourceView().Get()
	);
	m_pass_projector->SetInputResource(
		textures.g_buffer_a.GetShaderResourceView().Get(), // normal
		textures.g_buffer_depth.GetShaderResourceView().Get()
	);

	m_pass_post_process.SetInputResource(textures.frame_buffer_color.GetShaderResourceView().Get());

	//m_pass_bloom.SetInputTextureAdd(
	//	textures.frame_buffer_color.GetShaderResourceView().Get());
	//m_pass_bloom.SetInputTextureBlur(
	//	textures.emission.GetShaderResourceView().Get());
	m_pass_ssr.SetInputResource(
		&textures.ssr_textures,
		textures.g_buffer_a.GetShaderResourceView().Get(),
		textures.g_buffer_b.GetShaderResourceView().Get(),
		textures.g_buffer_c.GetShaderResourceView().Get(),
		textures.g_buffer_depth.GetShaderResourceView().Get(),
		textures.frame_buffer_color.GetShaderResourceView().Get()
		// m_pass_bloom.GetRt() // TODO
	);
	m_pass_blur_dual.SetInputResource(&textures.blur_textures, textures.emission.GetShaderResourceView().Get());
}

void RenderPathMain::Draw(RenderViewKey view_key, const ViewContext& view_context)
{
	auto it = m_texture_resources.find(view_key); // TODO: get by active camera
	assert(it != m_texture_resources.end());
	auto& textures = it->second;

	uint32_t width, height;
	view_context.render_target_out.GetSize(width, height);
	BuildRenderTargets(textures, width, height);

	ID3D11DeviceContext* context = GetDeviceContext();
	float clear_color[4]{ 0.0f, 0.0f, 0.0f, 0.0f }; // must be zero for proper blending

	m_gpu_timers[0].Start();

	// draw
	// depth pre-pass
	m_render_targets.pre_pass.ClearDepthStencil(context, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	m_render_targets.pre_pass.Bind(context);
	m_pass_depth_normal.Draw();
	m_render_targets.pre_pass.Unbind(context);

	// geometry
	m_render_targets.g_buffer.ClearColor(context, clear_color);
	// m_render_targets.g_buffer.ClearDepthStencil(context, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	m_render_targets.g_buffer.Bind(context);
	m_pass_geometry.Draw();
	m_render_targets.g_buffer.Unbind(context);

	m_render_targets.deferred_shading.ClearColor(context, clear_color);
	m_render_targets.deferred_shading.Bind(context);
	m_pass_deferred_shading.Draw();
	m_render_targets.deferred_shading.Unbind(context);

	m_gpu_timers[0].Stop();
	if (m_gpu_timers[0].TryGetTime(nullptr))
	{
		float avg_time = m_gpu_timers[0].AverageTime();
		hal::dout << "GPU time: deferred " << avg_time * 1000.0f << " ms" << std::endl;
	}

	m_gpu_timers[1].Start();
	m_render_targets.frame_buffer.Bind(context);
	m_pass_forward.SetDrawingQueue(PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.Draw();
	m_render_targets.frame_buffer.Unbind(context);
	// decal
	m_render_targets.deferred_shading.Bind(context);
	m_pass_forward.SetDrawingQueue(PassForward::SubPassQueue::DECAL);
	m_pass_forward.Draw();
	m_render_targets.deferred_shading.Unbind(context);

	m_render_targets.frame_buffer.Bind(context);
	m_pass_sky.Draw();
	m_pass_particle.Draw();
	m_render_targets.frame_buffer.Unbind(context);

	m_gpu_timers[1].Stop();
	if (m_gpu_timers[1].TryGetTime(nullptr))
	{
		float avg_time = m_gpu_timers[1].AverageTime();
		hal::dout << "GPU time: forward " << avg_time * 1000.0f << " ms" << std::endl;
	}

	m_gpu_timers[2].Start();

	// draw ssr
	m_pass_ssr.Draw();
	// add ssr
	int post_process_target_index{ 0 };
	m_render_targets.post_process_temps[post_process_target_index].ClearColor(context, clear_color);
	m_render_targets.post_process_temps[post_process_target_index].Bind(context);
	m_pass_composite.DrawAddReflection(
		textures.frame_buffer_color.GetShaderResourceView().Get(),
		textures.ssr_textures.texture_reflection_color.GetShaderResourceView().Get(),
		textures.g_buffer_b.GetShaderResourceView().Get()
	);
	m_render_targets.post_process_temps[post_process_target_index].Unbind(context);

	// draw blur
	m_pass_blur_dual.Draw();
	// add bloom
	post_process_target_index = 1 - post_process_target_index;
	m_render_targets.post_process_temps[post_process_target_index].ClearColor(context, clear_color);
	m_render_targets.post_process_temps[post_process_target_index].Bind(context);
	m_pass_composite.DrawAdd(
		textures.post_process_temps[1 - post_process_target_index].GetShaderResourceView().Get(),
		textures.blur_textures.mip_textures[0].GetShaderResourceView().Get());
	m_render_targets.post_process_temps[post_process_target_index].Unbind(context);

	m_gpu_timers[2].Stop();
	// m_pass_bloom.DrawOutput();
	if (m_gpu_timers[2].TryGetTime(nullptr))
	{
		float avg_time = m_gpu_timers[2].AverageTime();
		hal::dout << "GPU time: postprocess " << avg_time * 1000.0f << " ms" << std::endl;
	}

	// out
	view_context.render_target_out.ClearColor(context, clear_color);
	view_context.render_target_out.ClearDepthStencil(context, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	view_context.render_target_out.Bind(context);
	// convert to LDR
	m_pass_composite.DrawToneMapping(
		textures.post_process_temps[post_process_target_index].GetShaderResourceView().Get()
	);
	// UI
	m_pass_sprite.Draw();
	// view_context.render_target_out.Unbind(context);
}

void RenderPathMain::Finalize()
{
	// TODO
	// release textures
}