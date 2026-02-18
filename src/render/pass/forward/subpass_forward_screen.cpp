#include "subpass_forward_screen.h"

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
	struct MaterialScreen
	{
		// color
		DirectX::XMFLOAT3 emission_color{};
		float emission_intensity{};
		// size
		DirectX::XMFLOAT2 screen_pixels_scale{};
		// padding
		float _padding_0;
		float _padding_1;
	};
}

void SubPassForwardScreen::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_screen_ex.cso");
	m_cb_per_material = Buffer::CreateConstantBuffer(m_device, sizeof(MaterialScreen));
}

bool SubPassForwardScreen::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer)
		&& material_desc.IfTechnique(RenderTechnique::FORWARD_SCREEN, m_curr_render_layer);
}

void SubPassForwardScreen::SetInfoPerDraw()
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

void SubPassForwardScreen::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardScreen::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// material common
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(key.material_id);

	const auto& material_default = material_desc.GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();

	auto albedo = texture_loader.GetTexture(material_default.albedo_texture_id);
	m_context->PSSetShaderResources(0, 1, albedo.GetAddressOf());

	switch (material_default.cull_type)
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

	// screen specific
	// screen pixel texture
	const auto& material_screen = material_desc.GetTechnique<TechniqueDescForwardScreen>(m_curr_render_layer);
	auto screen_pixel = texture_loader.GetTexture(material_screen.screen_pixel_texture_id);
	m_context->PSSetShaderResources(1, 1, screen_pixel.GetAddressOf());
	auto texture_world = texture_loader.GetTexture(material_screen.world_texture_id);
	m_context->PSSetShaderResources(2, 1, texture_world.GetAddressOf());

	// constant buffer
	{
		MaterialScreen cb{};
		cb.emission_color = material_default.emission_color.ToXMFLOAT3();
		cb.emission_intensity = material_default.emission_intensity;
		cb.screen_pixels_scale = material_screen.screen_pixels_scale.ToXMFLOAT2();
		m_context->UpdateSubresource(m_cb_per_material.Get(), 0, nullptr, &cb, 0, 0);
		m_context->PSSetConstantBuffers(0, 1, m_cb_per_material.GetAddressOf());
	}
}
