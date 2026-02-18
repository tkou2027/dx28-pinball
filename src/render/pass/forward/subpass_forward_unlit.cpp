#include "subpass_forward_unlit.h"

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


void SubPassForwardUnlit::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_unlit.cso");
}

bool SubPassForwardUnlit::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer)
		&& material_desc.IfTechnique(RenderTechnique::FORWARD_UNLIT, m_curr_render_layer);
}

void SubPassForwardUnlit::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardUnlit::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// per object material
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material = material_resource.GetMaterialDesc(key.material_id).GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();
	auto albedo = texture_loader.GetTexture(material.albedo_texture_id);
	m_context->PSSetShaderResources(0, 1, albedo.GetAddressOf());
	auto emission = texture_loader.GetTexture(material.emission_texture_id);
	m_context->PSSetShaderResources(1, 1, emission.GetAddressOf());
	switch (material.cull_type)
	{
	case CullType::CULL_BACK:
	{
		m_context->RSSetState(render_states.m_rs_cull_back.Get());
		break;
	}
	case CullType::CULL_FRONT:
	{
		m_context->RSSetState(render_states.m_rs_cull_front.Get());
		break;
	}
	case CullType::CULL_NONE:
	{
		m_context->RSSetState(render_states.m_rs_cull_none.Get());
		break;
	}
	}
}

void SubPassForwardUnlit::SetInfoPerDraw()
{
	PassBaseGeometry::SetInfoPerDraw(); // mvp matrix

	const auto& render_states = GetRenderStates();
	// ps
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}
