#include "pass_ssr_legacy.h"
#include "render/render_states.h"
#include "render/render_resource.h"
#include <DirectXMath.h>


#include "global_context.h"
#include "render/render_system.h"

#include "shader_setting.h"

using namespace DirectX;

namespace
{
	struct SSRConfig
	{
		float max_distance;
		float resolution;
		float thickness;
		INT steps;
	};
}

void PassSSRLegacy::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);

	m_vs = Shader::CreateShaderVertex(
		m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC);
	m_ps_uv = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_ssr.cso");
	m_ps_blur = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_box_blur.cso");

	m_rt_reflection.Initialize(device, context, GetScreenWidth(), GetScreenHeight());
	m_rt_reflection.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET); // reflection info
	m_rt_reflection.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET_HDR); // reflection color

	m_rt_blur.Initialize(device, context, GetScreenWidth(), GetScreenHeight());
	m_rt_blur.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET_HDR);

	m_cb_ssr = CreateConstantBuffer(sizeof(SSRConfig));
}

void PassSSRLegacy::Draw()
{
	DrawUVTexture();
	DrawBlur();
}

void PassSSRLegacy::SetInputResource(ID3D11ShaderResourceView* g_buffer_a, ID3D11ShaderResourceView* g_buffer_b, ID3D11ShaderResourceView* g_buffer_c, ID3D11ShaderResourceView* g_buffer_depth, ID3D11ShaderResourceView* color_texture)
{
	m_srv_g_buffer[0] = g_buffer_a;
	m_srv_g_buffer[1] = g_buffer_b;
	m_srv_g_buffer[2] = g_buffer_c;
	m_srv_g_buffer[3] = g_buffer_depth;
	m_srv_color_texture = color_texture;
}

void PassSSRLegacy::SetGBuffer(const RenderAttachment& geo_buffer)
{
	for (int i = 0; i < BUFFER_NUM - 1; i++)
	{
		m_srv_g_buffer[i] = geo_buffer.GetTextureColor<TextureResource2D>(i)->GetShaderResourceView().Get();
	}
	// depth stencil
	m_srv_g_buffer[BUFFER_NUM - 1] = geo_buffer.GetTextureDepthStencil()->GetShaderResourceView().Get();
}

void PassSSRLegacy::SetColorTexture(ID3D11ShaderResourceView* srv)
{
	m_srv_color_texture = srv;
}

void PassSSRLegacy::DrawUVTexture()
{
	float black[4] = { 0.0f ,0.0f, 0.0f, 0.0f };
	m_rt_reflection.ClearColor(black);
	m_rt_reflection.Bind();
	m_context->IASetInputLayout(m_vs.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps_uv.Get(), nullptr, 0);
	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// rs
	m_context->RSSetState(render_states.m_rs_cull_none.Get());
	// ps
	m_context->PSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->PSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->PSSetConstantBuffers(2, 1, m_cb_ssr.GetAddressOf());
	{
		SSRConfig cb{};
		cb.max_distance = g_shader_setting.ssr_setting.max_distance;
		cb.resolution = g_shader_setting.ssr_setting.resolution;
		cb.thickness = g_shader_setting.ssr_setting.thickness;
		cb.steps = g_shader_setting.ssr_setting.steps;
		m_context->UpdateSubresource(m_cb_ssr.Get(), 0, nullptr, &cb, 0, 0);
	}
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, BUFFER_NUM, m_srv_g_buffer);
	m_context->PSSetShaderResources(BUFFER_NUM, 1, &m_srv_color_texture);
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);

	m_context->Draw(3, 0);
}


void PassSSRLegacy::DrawBlur()
{
	float black[4] = { 0.0f ,0.0f, 0.0f, 0.0f };
	m_rt_blur.ClearColor(black);
	m_rt_blur.Bind();
	m_context->IASetInputLayout(m_vs.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps_blur.Get(), nullptr, 0);
	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// rs
	m_context->RSSetState(render_states.m_rs_cull_none.Get());
	// ps
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, 1, m_rt_reflection.GetTextureColor<TextureResource2D>(1)->GetShaderResourceView().GetAddressOf());
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);

	m_context->Draw(3, 0);
}

void PassSSRLegacy::DrawBlend()
{

}
