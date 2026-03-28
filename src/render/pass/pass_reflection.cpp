#include "pass_reflection.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "render/shader_setting.h"
#include "render/util/dx_trace.h"

void PassReflection::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	VertexCommon::VertexCommonConfig vertex_config{};
	vertex_config.shader_configs[static_cast<size_t>(ModelType::STATIC)]
		= { L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC }; // TODO
	PassBaseGeometry::Initialize(device, context, vertex_config);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_reflection_plane.cso");
}

void PassReflection::SetInputResource(
	ID3D11ShaderResourceView* buffer_normal,
	ID3D11ShaderResourceView* buffer_material,
	ID3D11ShaderResourceView* buffer_depth
)
{
	m_buffer_normal = buffer_normal;
	m_buffer_material = buffer_material;
	m_buffer_depth = buffer_depth;
}

bool PassReflection::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::FORWARD_REFLECTION_PLANE, m_curr_render_layer);
}

void PassReflection::SetInfoPerDraw()
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
	m_context->PSSetShaderResources(0, 1, &m_buffer_normal); // normal
	m_context->PSSetShaderResources(1, 1, &m_buffer_material); // material
	m_context->PSSetShaderResources(2, 1, &m_buffer_depth); // get position form depth
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0); // read only
	// blend add
	m_context->OMSetBlendState(render_states.m_bs_add.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}

void PassReflection::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void PassReflection::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// material common
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(key.material_id);
	const auto& material_reflection = material_desc.GetTechnique<TechniqueDescForwardReflectionPlane>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();
	const auto texture_srv = texture_loader.GetTexture(material_reflection.texture_id);
	m_context->PSSetShaderResources(3, 1, texture_srv.GetAddressOf());
	// screen specific
	// screen pixel texture
}
