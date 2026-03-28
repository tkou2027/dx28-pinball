#include "subpass_forward_screen_projection.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "render/shader_setting.h"
#include "render/util/dx_trace.h"

namespace
{
	struct MaterialScreen
	{
		DirectX::XMFLOAT4X4 projector_mat_view_proj{};
		DirectX::XMFLOAT3 position_w_top_left;
		float _padding_position_w_top_left;
		DirectX::XMFLOAT3 position_w_bottom_right;
		float _padding_position_w_bottom_right;
	};
}

void SubPassForwardScreenProjection::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	VertexCommon::VertexCommonConfig vertex_config{};
	vertex_config.shader_configs[static_cast<size_t>(ModelType::STATIC)]
		= { L"vertex_geometry_static.cso", Shader::InputLayoutType::MESH_STATIC };
	PassBaseGeometry::Initialize(device, context, vertex_config);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_screen_projection.cso");
	m_cb_per_material = Buffer::CreateConstantBuffer(m_device, sizeof(MaterialScreen));
}

bool SubPassForwardScreenProjection::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::FORWARD_SCREEN_PROJECTION, m_curr_render_layer);
}

void SubPassForwardScreenProjection::SetInfoPerDraw()
{
	PassBaseGeometry::SetInfoPerDraw(); // mvp matrix

	const auto& render_states = GetRenderStates();
	// ps
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	m_context->PSSetSamplers(1, 1, render_states.m_ss_point_wrap.GetAddressOf());
	m_context->PSSetConstantBuffers(0, 1, m_cb_per_material.GetAddressOf());
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}

void SubPassForwardScreenProjection::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardScreenProjection::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// material common
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(key.material_id);

	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();

	//auto albedo = texture_loader.GetTexture(material_default.albedo_texture_id);
	//m_context->PSSetShaderResources(0, 1, albedo.GetAddressOf());

	m_context->RSSetState(render_states.m_rs_cull_none.Get());
	// screen specific
	// screen pixel texture
	const auto& material_screen = material_desc.GetTechnique<TechniqueDescForwardScreenProjection>(m_curr_render_layer);
	auto texture_world = texture_loader.GetTexture(material_screen.world_texture_id);
	m_context->PSSetShaderResources(0, 1, texture_world.GetAddressOf());

	// constant buffer
	{
		MaterialScreen cb{};
		cb.projector_mat_view_proj = material_screen.projector_mat_view_proj;
		cb.position_w_top_left = material_screen.position_w_top_left.ToXMFLOAT3();
		cb.position_w_bottom_right = material_screen.position_w_bottom_right.ToXMFLOAT3();
		m_context->UpdateSubresource(m_cb_per_material.Get(), 0, nullptr, &cb, 0, 0);
	}
}
