#include "subpass_forward_screen_background.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"
#include "config/palette.h"
#include "shader_setting.h"
#include "render/dx_trace.h"

namespace
{
	struct ScreenBGConfig
	{
		float center_u;
		float center_v;
		float radius;
		float thickness;

		float time;
		float rotation;
		float stripe_density;
		float stripe_offset;

		DirectX::XMFLOAT4 color_inside;
		DirectX::XMFLOAT4 color_stripe_positive;
		DirectX::XMFLOAT4 color_stripe_negative;
	};
}

void SubPassForwardScreenBackground::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// static mesh only (TODO: no uv / color)
	VertexCommon::VertexCommonConfig vertex_config{};
	vertex_config.shader_configs[static_cast<size_t>(ModelType::STATIC)]
		= { L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC };
	PassBaseGeometry::Initialize(device, context, vertex_config);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_screen_background.cso");
	m_cb = Buffer::CreateConstantBuffer(m_device, sizeof(ScreenBGConfig));
}

bool SubPassForwardScreenBackground::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::FORWARD_SCREEN_BACKGROUND, m_curr_render_layer);
}

void SubPassForwardScreenBackground::SetInfoPerDraw()
{
	PassBaseGeometry::SetInfoPerDraw(); // mvp matrix

	const auto& render_states = GetRenderStates();
	// rs
	m_context->RSSetState(render_states.m_rs_cull_none.Get());
	// ps
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	m_context->PSSetSamplers(1, 1, render_states.m_ss_point_wrap.GetAddressOf());
	m_context->PSSetConstantBuffers(0, 1, m_cb.GetAddressOf());
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}

void SubPassForwardScreenBackground::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardScreenBackground::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// material common
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(key.material_id);
	const auto& material_screen = material_desc.GetTechnique<TechniqueDescForwardScreenBackground>(m_curr_render_layer);
	// constant buffer
	{
		ScreenBGConfig cb{};

		cb.center_u = material_screen.center_u;
		cb.center_v = material_screen.center_v;
		cb.radius = material_screen.radius;
		cb.thickness = material_screen.thickness;
		cb.time = material_screen.time;
		cb.rotation = material_screen.rotation;
		cb.stripe_density = material_screen.stripe_density;
		cb.stripe_offset = material_screen.stripe_offset;
		cb.color_inside = material_screen.color_inside.ToXMFLOAT4();
		cb.color_stripe_positive = material_screen.color_stripe_positive.ToXMFLOAT4();
		cb.color_stripe_negative = material_screen.color_stripe_negative.ToXMFLOAT4();
		m_context->UpdateSubresource(m_cb.Get(), 0, nullptr, &cb, 0, 0);
	}
}
