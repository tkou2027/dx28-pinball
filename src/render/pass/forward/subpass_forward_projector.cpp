#include "subpass_forward_projector.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"
#include "render/dx_trace.h"

namespace
{
	struct MaterialProjector
	{
		// projector space
		DirectX::XMFLOAT4X4 projector_matrix_view_proj;
		DirectX::XMFLOAT3 projector_position_w;
		float _padding;
		// texture
	};
}

void SubPassForwardProjector::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{

	VertexCommon::VertexCommonConfig vertex_config{};
	vertex_config.shader_configs[static_cast<size_t>(ModelType::STATIC)]
		= { L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC };
	PassBaseGeometry::Initialize(device, context, vertex_config);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_projector.cso");
	m_cb_per_material = Buffer::CreateConstantBuffer(m_device, sizeof(MaterialProjector));
}

void SubPassForwardProjector::SetInputResource(ID3D11ShaderResourceView* buffer_normal, ID3D11ShaderResourceView* buffer_depth)
{
	m_buffer_normal = buffer_normal;
	m_buffer_depth = buffer_depth;
}

bool SubPassForwardProjector::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::FORWARD_PROJECTOR, m_curr_render_layer);
}

void SubPassForwardProjector::SetInfoPerDraw()
{
	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// rs
	m_context->RSSetState(render_states.m_rs_cull_none.Get());
	// ps
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	// view matrix for reverse world matrix
	m_context->PSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->PSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->PSSetConstantBuffers(2, 1, m_cb_per_material.GetAddressOf());
	m_context->PSSetShaderResources(0, 1, &m_buffer_normal); // normal
	m_context->PSSetShaderResources(1, 1, &m_buffer_depth); // get position form depth
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0); // TODO
	// blend add
	m_context->OMSetBlendState(render_states.m_bs_add.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}

void SubPassForwardProjector::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardProjector::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// material common
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(key.material_id);
	const auto& material_projector = material_desc.GetTechnique<TechniqueDescForwardProjector>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();
	const auto texture_srv = texture_loader.GetTexture(material_projector.projectior_texture_id);
	m_context->PSSetShaderResources(2, 1, texture_srv.GetAddressOf());

	// screen specific
	// screen pixel texture
	
	// constant buffer
	{
		MaterialProjector cb{};
		cb.projector_matrix_view_proj = material_projector.projector_mat_view_proj;
		cb.projector_position_w = material_projector.projector_position_w.ToXMFLOAT3();
		m_context->UpdateSubresource(m_cb_per_material.Get(), 0, nullptr, &cb, 0, 0);
		m_context->PSSetConstantBuffers(2, 1, m_cb_per_material.GetAddressOf());
	}
}
