#include "pass_depth.h"

#include <DirectXMath.h>
#include "config/constant.h"
#include "platform/graphics.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"
#include "render/dx_trace.h"

using namespace DirectX;

void PassDepth::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
	m_pixel_shader = Shader::CreateShaderPixel(m_device, L"pixel_geometry.cso");
}

bool PassDepth::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	if (!material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer))
	{
		return false;
	}
	const auto& material = material_desc.GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	return material.pre_pass;
}

void PassDepth::SetInfoPerDraw()
{
	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// vs
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->VSSetConstantBuffers(2, 1, render_resource.m_buffer_per_mesh.GetAddressOf());
	// ps (none)
	m_context->PSSetShader(nullptr, nullptr, 0);
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}

void PassDepth::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void PassDepth::SetInfoPerMaterial(const ModelRenderKey& model_key)
{
	// per object material
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(model_key.material_id);
	const auto& material = material_desc.GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	// states
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
