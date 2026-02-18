#include "render/path/render_path_screen_content.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include "util/debug_ostream.h"
#include "render/pass/forward/subpass_forward_screen_background.h"

void RenderPathScreenContent::Initialize()
{
	ID3D11Device* device = GetDevice();
	ID3D11DeviceContext* context = GetDeviceContext();

	m_pass_forward.Initialize(device, context);
	m_pass_forward.AddSubPass(std::make_unique<SubPassForwardScreenBackground>(), PassForward::SubPassQueue::DEFAULT);
	m_pass_sprite.Initialize(device, context);

	for (auto& timer : m_gpu_timers)
	{
		timer.Initialize(device, context);
	}
}

void RenderPathScreenContent::InitializeViewContext(RenderViewKey view_key, uint32_t width, uint32_t height) {}

void RenderPathScreenContent::UpdateVisibleRenderables(
	const SceneRenderablesManager& scene_renderables, CameraRenderLayer render_layer)
{
	const auto& material_resource = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	const auto& visible_info = scene_renderables.GetRenderablesOfLayer(render_layer);

	m_pass_sprite.SetRenderLayer(render_layer);
	m_pass_forward.ResetRenderableIndices(render_layer);
	for (const auto& index : visible_info.indices_model)
	{
		const auto& model_info = scene_renderables.m_models[index];
		const auto& material = material_resource.GetMaterialDesc(model_info.key.material_id);
		m_pass_forward.AddRenderableIndex(index, model_info.key.model_type, material);
	}
}

void RenderPathScreenContent::Draw(RenderViewKey view_key, const ViewContext& view_context)
{
	ID3D11DeviceContext* context = GetDeviceContext();
	float clear_color[4]{ 0.0f, 0.0f, 0.0f, 1.0f }; // black background
	// out
	view_context.render_target_out.ClearColor(context, clear_color);
	view_context.render_target_out.ClearDepthStencil(context, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	view_context.render_target_out.Bind(context);
	m_pass_sprite.Draw();
	m_pass_forward.SetDrawingQueue(PassForward::SubPassQueue::DEFAULT);
	m_pass_forward.Draw();
}

void RenderPathScreenContent::Finalize()
{
	// no textures to use
}