#include "subpass_forward_silhouette.h"

#include <DirectXMath.h>

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"
#include "render/dx_trace.h"


void SubPassForwardSilhouette::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_silhouette.cso");
}

bool SubPassForwardSilhouette::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer)
		&& material_desc.IfTechnique(RenderTechnique::FORWARD_SILHOUETTE, m_curr_render_layer);
}

void SubPassForwardSilhouette::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardSilhouette::Draw()
{
	SetInfoPerDrawCommon();
	m_draw_pass = DrawPass::MASK;
	PassBaseGeometry::Draw();
	m_draw_pass = DrawPass::DRAW;
	PassBaseGeometry::Draw();
}

void SubPassForwardSilhouette::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// per object material
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material = material_resource.GetMaterialDesc(key.material_id).GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();
	auto albedo = texture_loader.GetTexture(material.albedo_texture_id);
	m_context->PSSetShaderResources(0, 1, albedo.GetAddressOf());
}

void SubPassForwardSilhouette::SetInfoPerDraw()
{
	// called by base draw
	const auto& render_states = GetRenderStates();
	switch (m_draw_pass)
	{
	case DrawPass::MASK:
	{
		m_context->RSSetState(render_states.m_rs_cull_back.Get());
		m_context->PSSetShader(nullptr, nullptr, 0);
		// TODO: config
		m_context->OMSetDepthStencilState(GetRenderStates().m_dss_depth_read_stencil_write.Get(), 1);
		break;
	}
	case DrawPass::DRAW:
	{
		m_context->RSSetState(render_states.m_rs_cull_front.Get());
		m_context->PSSetShader(m_ps.Get(), nullptr, 0);
		m_context->OMSetDepthStencilState(GetRenderStates().m_dss_depth_read_greater_stencil_read_neq.Get(), 1);
		break;
	}
	}
}

void SubPassForwardSilhouette::SetInfoPerDrawCommon()
{
	PassBaseGeometry::SetInfoPerDraw(); // mvp matrix
	const auto& render_states = GetRenderStates();
	// ps
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	// om
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}
