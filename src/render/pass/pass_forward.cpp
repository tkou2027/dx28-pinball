#include "pass_forward.h"

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

void PassForward::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
}

void PassForward::Draw()
{
	const auto& queue = m_queues[static_cast<size_t>(m_drawing_queue)];
	for (const auto& subpass : queue)
	{
		subpass->Draw();
	}
}

void PassForward::AddSubPass(std::unique_ptr<PassBaseGeometry> subpass, SubPassQueue queue)
{
	// subpass->SetPassForward(this);
	subpass->Initialize(m_device, m_context);
	auto& subpasses = m_queues[static_cast<size_t>(queue)];
	subpasses.push_back(std::move(subpass));
}

void PassForward::ResetRenderableIndices(CameraRenderLayer layer)
{
	for (auto& queue : m_queues)
	{
		for (auto& subpass : queue)
		{
			subpass->ResetRenderableIndices(layer);
		}
	}
}

void PassForward::AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc)
{
	for (auto& queue : m_queues)
	{
		for (auto& subpass : queue)
		{
			subpass->AddRenderableIndex(mesh_index, model_type, material_desc);
		}
	}
}

// glass
void SubPassForwardGlass::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_glass.cso");
}

bool SubPassForwardGlass::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer)
		&& material_desc.IfTechnique(RenderTechnique::FORWARD_GLASS, m_curr_render_layer);
}

void SubPassForwardGlass::SetInfoPerModel(const ModelRenderInfo& model_info)
{
	PassBaseGeometry::SetInfoPerModel(model_info);
	SetInfoPerMaterial(model_info.key);
}

void SubPassForwardGlass::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// per object material
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(key.material_id);

	const auto& material = material_desc.GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();

	auto& texture_loader = GetTextureLoader();
	const auto& material_glass = material_desc.GetTechnique<TechniqueDescForwardGlass>(m_curr_render_layer);
	auto evn_map = texture_loader.GetTexture(material_glass.env_texture_id);
	m_context->PSSetShaderResources(2, 1, evn_map.GetAddressOf());
	// if (material.use_albedo_texture)
	// {
	// 	auto albedo = texture_loader.GetTexture(material.albedo_texture_id);
	// 	m_context->PSSetShaderResources(0, 1, albedo.GetAddressOf());
	// }
	// if (material.use_emission_texture)
	// {
	// 	auto emission = texture_loader.GetTexture(material.emission_texture_id);
	// 	m_context->PSSetShaderResources(1, 1, emission.GetAddressOf());
	// }
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

void SubPassForwardGlass::SetInfoPerDraw()
{
	PassBaseGeometry::SetInfoPerDraw(); // mvp matrix

	const auto& render_states = GetRenderStates();
	// ps
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}
