#include "render/path/render_path_screen_reflect.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include <chrono>
#include "util/debug_ostream.h"

void RenderPathScreenReflect::Initialize()
{
	ID3D11Device* device = GetDevice();
	ID3D11DeviceContext* context = GetDeviceContext();

	// initialize passes
	m_pass_forward.Initialize(device, context);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardUnlit>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardScreen>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_post_process.Initialize(device, context);

	for (auto& timer : m_gpu_timers)
	{
		timer.Initialize(device, context);
	}
}

void RenderPathScreenReflect::InitializeViewContext(RenderViewKey view_key, uint32_t width, uint32_t height)
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
	// frame
	textures.frame_buffer_color.InitializeRenderTarget2D(device, render_target_hdr_desc);
	textures.emission.InitializeRenderTarget2D(device, render_target_hdr_desc);
	textures.frame_buffer_depth.InitializeDepth2D(device, depth_stencil_desc);

	m_texture_resources.emplace(view_key, textures);
}

void RenderPathScreenReflect::UpdateVisibleRenderables(
	const SceneRenderablesManager& scene_renderables, CameraRenderLayer render_layer)
{
	// static int evil_cnt{ 0 };
	// if (evil_cnt == 1)
	// {
	// 	evil_cnt = 0;
	// 	return;
	// }
	// evil_cnt = 1;

	auto start = std::chrono::high_resolution_clock::now();

	const auto& material_resource = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	const auto& visible_info = scene_renderables.GetRenderablesOfLayer(render_layer);

	m_pass_forward.ResetRenderableIndices(render_layer);

	// models
	for (const auto& index : visible_info.indices_model)
	{
		const auto& model_info = scene_renderables.m_models[index];
		const auto& material = material_resource.GetMaterialDesc(model_info.key.material_id);
		m_pass_forward.AddRenderableIndex(index, model_info.key.model_type, material);
	}

	auto finish = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "updating view" << duration.count() << " " << std::endl;
}

void RenderPathScreenReflect::BuildRenderTargets(const InternalTextures& textures, uint32_t width, uint32_t height)
{
	m_render_targets.frame_buffer.Reset(width, height);
	m_render_targets.frame_buffer.AddRenderTarget(textures.frame_buffer_color.GetRenderTargetView());
	m_render_targets.frame_buffer.AddRenderTarget(textures.emission.GetRenderTargetView());
	m_render_targets.frame_buffer.SetDepthStencil(textures.frame_buffer_depth.GetDepthStencilView());

	// initialize resource inputs
	m_pass_post_process.SetInputResource(textures.frame_buffer_color.GetShaderResourceView().Get());
}

void RenderPathScreenReflect::Draw(RenderViewKey view_key, const ViewContext& view_context)
{
	auto it = m_texture_resources.find(view_key); // TODO: get by active camera
	assert(it != m_texture_resources.end());
	auto& textures = it->second;

	uint32_t width, height;
	view_context.render_target_out.GetSize(width, height);
	BuildRenderTargets(textures, width, height);

	ID3D11DeviceContext* context = GetDeviceContext();
	float clear_color[4]{ 0.0f, 0.0f, 0.0f, 0.0f }; // must be zero for proper blending

	// shading
	m_render_targets.frame_buffer.ClearColor(context, clear_color);
	m_render_targets.frame_buffer.ClearDepthStencil(context, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	m_render_targets.frame_buffer.Bind(context);
	m_gpu_timers[1].Start();

	m_pass_forward.SetDrawingQueue(PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.Draw();

	m_gpu_timers[1].Stop();
	if (m_gpu_timers[1].TryGetTime(nullptr))
	{
		float avg_time = m_gpu_timers[1].AverageTime();
		hal::dout << "GPU time: forward " << avg_time * 1000.0f << " ms" << std::endl;
	}

	// out
	view_context.render_target_out.ClearColor(context, clear_color);
	view_context.render_target_out.ClearDepthStencil(context, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	view_context.render_target_out.Bind(context);
	m_pass_post_process.Draw();

	// reset
	ID3D11ShaderResourceView* nullSRVs[4] = { nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 4, nullSRVs);
	context->OMSetRenderTargets(0, nullptr, nullptr);
}

void RenderPathScreenReflect::Finalize()
{
	// TODO
	// release textures
}