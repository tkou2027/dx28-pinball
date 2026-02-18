#include "pass_ssr.h"
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

void PassSSR::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);

	m_vs = Shader::CreateShaderVertex(
		m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::POS);
	m_ps_uv = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_ssr.cso");
	m_ps_blur = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_box_blur.cso");

	m_cb_ssr = CreateConstantBuffer(sizeof(SSRConfig));
}

void PassSSR::Draw()
{
	DrawUVTexture();
	DrawBlur();
}

void PassSSR::CreateTextures(uint32_t width, uint32_t height, InternalTextures& out_textures)
{
	{
		auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET;
		desc.Width = width;
		desc.Height = height;
		out_textures.texture_reflection_info.InitializeRenderTarget2D(m_device, desc);
	}
	{
		auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET_HDR;
		desc.Width = width;
		desc.Height = height;
		// auto mip map
		desc.MipLevels = 0; // full mip map
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		out_textures.texture_reflection_color.InitializeRenderTarget2D(m_device, desc);
	}
	{
		auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET_HDR;
		desc.Width = width;
		desc.Height = height;
		out_textures.texture_reflection_blurred.InitializeRenderTarget2D(m_device, desc);
	}
}

void PassSSR::SetInputResource(const InternalTextures* textures, ID3D11ShaderResourceView* g_buffer_a, ID3D11ShaderResourceView* g_buffer_b, ID3D11ShaderResourceView* g_buffer_c, ID3D11ShaderResourceView* g_buffer_depth, ID3D11ShaderResourceView* color_texture)
{
	m_textures = textures;
	m_srv_g_buffer[0] = g_buffer_a;
	m_srv_g_buffer[1] = g_buffer_b;
	m_srv_g_buffer[2] = g_buffer_c;
	m_srv_g_buffer[3] = g_buffer_depth;
	m_srv_color_texture = color_texture;
}

void PassSSR::DrawUVTexture()
{
	RenderTarget rt{};
	rt.Initialize(m_textures->texture_reflection_info.GetWidth(), m_textures->texture_reflection_info.GetHeight());
	rt.AddRenderTarget(m_textures->texture_reflection_info.GetRenderTargetView());
	rt.AddRenderTarget(m_textures->texture_reflection_color.GetRenderTargetView());
	float no_color[4] = { 0.0f ,0.0f, 0.0f, 0.0f };
	rt.ClearColor(m_context, no_color);
	rt.Bind(m_context);

	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->PSSetShader(m_ps_uv.Get(), nullptr, 0);
	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// rs
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
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

	m_context->Draw(3, 0);
	rt.Unbind(m_context);
}


void PassSSR::DrawBlur()
{
	m_context->GenerateMips(m_textures->texture_reflection_color.GetShaderResourceView().Get());
	return;
	// TODO manual mip map generation

	//RenderTarget rt{};
	//rt.Initialize(m_textures->texture_reflection_blurred.GetWidth(), m_textures->texture_reflection_color.GetHeight());
	//rt.AddRenderTarget(m_textures->texture_reflection_blurred.GetRenderTargetView());
	//float no_color[4] = { 0.0f ,0.0f, 0.0f, 0.0f };
	//rt.ClearColor(m_context, no_color);
	//rt.Bind(m_context);

	//m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	//m_context->IASetInputLayout(m_vs.input_layout.Get());
	//m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_context->PSSetShader(m_ps_blur.Get(), nullptr, 0);
	//const auto& render_states = GetRenderStates();
	//const auto& render_resource = GetRenderResource();
	//// rs
	//m_context->RSSetState(render_states.m_rs_cull_back.Get());
	//// ps
	//m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	//m_context->PSSetShaderResources(0, 1, m_textures->texture_reflection_color.GetShaderResourceView().GetAddressOf());
	//// om
	//m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	//m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
	//	render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	//m_context->Draw(3, 0);
	//rt.Unbind(m_context);
}