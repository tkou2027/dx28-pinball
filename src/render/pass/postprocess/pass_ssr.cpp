#include "pass_ssr.h"
#include "render/render_states.h"
#include "render/render_resource.h"
#include <DirectXMath.h>


#include "global_context.h"
#include "render/render_system.h"
#include "render/shader_setting.h"

using namespace DirectX;

namespace
{
	struct SSRConfig
	{
		float max_distance;
		float stride;
		float thickness;
		INT steps;
	};
	struct HiZConfig
	{
		UINT width;
		UINT height;
		UINT width_in;
		UINT height_in;
	};
}

void PassSSR::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);

	m_cs_hi_z_copy = Shader::CreateShaderCompute(m_device, L"compute_hi_z_copy.cso");
	m_cs_hi_z_gen = Shader::CreateShaderCompute(m_device, L"compute_hi_z_generate.cso");
	m_vs = Shader::CreateShaderVertex(
		m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::POS);
	m_ps_uv = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_ssr.cso");
	m_cs_refine = Shader::CreateShaderCompute(m_device, L"compute_ssr_refine.cso");
	// m_ps_blur = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_box_blur.cso");

	m_cb_ssr = CreateConstantBuffer(sizeof(SSRConfig));
	m_cb_hi_z = CreateConstantBuffer(sizeof(HiZConfig));
}

void PassSSR::Draw()
{
	DrawHiZ();
	DrawUVTexture();
	DrawRefine();
	DrawBlur();
}

void PassSSR::CreateTextures(uint32_t width, uint32_t height, InternalTextures& out_textures)
{
	{
		auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET;
		desc.Width = width / 2;
		desc.Height = height / 2;
		out_textures.texture_reflection_info.InitializeRenderTarget2D(m_device, desc);
	}
	{
		auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET_HDR;
		desc.Width = width / 2;
		desc.Height = height / 2;
		out_textures.texture_reflection_color_raw.InitializeRenderTarget2D(m_device, desc);
	}
	{
		auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET_HDR;
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS; // for compute shader access
		desc.Width = width / 2;
		desc.Height = height / 2;
		// auto mip map
		desc.MipLevels = 0; // full mip map
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		out_textures.texture_reflection_color.InitializeUnorderedAccess2D(m_device, desc);
	}
	{
		CD3D11_TEXTURE2D_DESC desc_hi_z{};
		desc_hi_z.Width = width;
		desc_hi_z.Height = height;
		desc_hi_z.MipLevels = 8; // auto mip map levels(??)
		desc_hi_z.ArraySize = 1;
		desc_hi_z.Format = DXGI_FORMAT_R32_FLOAT;
		desc_hi_z.SampleDesc.Count = 1; // no msaa
		desc_hi_z.SampleDesc.Quality = 0;
		desc_hi_z.Usage = D3D11_USAGE_DEFAULT;
		desc_hi_z.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc_hi_z.CPUAccessFlags = 0;
		desc_hi_z.MiscFlags = 0;
		desc_hi_z.Width = width;
		desc_hi_z.Height = height;
		out_textures.texture_hi_z.Initialize(m_device, desc_hi_z);
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

void PassSSR::DrawHiZ()
{
	auto& texture_hi_z = m_textures->texture_hi_z;
	uint32_t mip_count = texture_hi_z.GetMipCount();

	// copy depth to Hi-Z Mip 0
	{
		m_context->CSSetShader(m_cs_hi_z_copy.Get(), nullptr, 0);

		HiZConfig cb{};
		cb.width = texture_hi_z.GetWidth();
		cb.height = texture_hi_z.GetHeight();
		cb.width_in = cb.width;
		cb.height_in = cb.height;
		m_context->UpdateSubresource(m_cb_hi_z.Get(), 0, nullptr, &cb, 0, 0);

		m_context->CSSetConstantBuffers(0, 1, m_cb_hi_z.GetAddressOf());
		m_context->CSSetShaderResources(0, 1, &m_srv_g_buffer[3]); // depth
		const auto& uav = texture_hi_z.GetUAVOfLevel(0);
		m_context->CSSetUnorderedAccessViews(0, 1, uav.GetAddressOf(), nullptr);

		m_context->Dispatch((cb.width + 15) / 16, (cb.height + 15) / 16, 1);

		// unbind
		ID3D11UnorderedAccessView* null_uav = nullptr;
		m_context->CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);
	}

	// generate Hi-Z Mip
	{
		m_context->CSSetShader(m_cs_hi_z_gen.Get(), nullptr, 0);

		uint32_t width_out = texture_hi_z.GetWidth();
		uint32_t height_out = texture_hi_z.GetHeight();
		for (uint32_t i = 1; i < mip_count; ++i)
		{
			uint32_t width_in = width_out;
			uint32_t height_in = height_out;
			width_out = std::max(1u, width_out / 2);
			height_out = std::max(1u, height_out / 2);

			HiZConfig cb{};
			cb.width = width_out;
			cb.height = height_out;
			cb.width_in = width_in;
			cb.height_in = height_in;
			m_context->UpdateSubresource(m_cb_hi_z.Get(), 0, nullptr, &cb, 0, 0);

			auto srv_in = texture_hi_z.GetSRVOfLevel(i - 1);
			m_context->CSSetShaderResources(0, 1, srv_in.GetAddressOf());
			auto uav_out = texture_hi_z.GetUAVOfLevel(i);
			m_context->CSSetUnorderedAccessViews(0, 1, uav_out.GetAddressOf(), nullptr);

			m_context->Dispatch((width_out + 15) / 16, (height_out + 15) / 16, 1);

			// unbind
			ID3D11UnorderedAccessView* null_uav = nullptr;
			m_context->CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);
		}
	}
}

void PassSSR::DrawUVTexture()
{
	RenderTarget rt{};
	rt.Initialize(m_textures->texture_reflection_info.GetWidth(), m_textures->texture_reflection_info.GetHeight());
	rt.AddRenderTarget(m_textures->texture_reflection_info.GetRenderTargetView());
	rt.AddRenderTarget(m_textures->texture_reflection_color_raw.GetRenderTargetView());
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
		cb.stride = g_shader_setting.ssr_setting.stride;
		cb.thickness = g_shader_setting.ssr_setting.thickness;
		cb.steps = g_shader_setting.ssr_setting.steps;
		m_context->UpdateSubresource(m_cb_ssr.Get(), 0, nullptr, &cb, 0, 0);
	}
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetSamplers(1, 1, render_states.m_ss_point_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, BUFFER_NUM, m_srv_g_buffer);
	m_context->PSSetShaderResources(BUFFER_NUM, 1, &m_srv_color_texture);
	m_context->PSSetShaderResources(BUFFER_NUM + 1, 1, m_textures->texture_hi_z.GetShaderResourceView().GetAddressOf());

	m_context->Draw(3, 0);
	rt.Unbind(m_context);
}

void PassSSR::DrawRefine()
{
	m_context->CSSetShader(m_cs_refine.Get(), nullptr, 0);
	auto srv_in = m_textures->texture_reflection_color_raw.GetShaderResourceView();
	m_context->CSSetShaderResources(0, 1, srv_in.GetAddressOf());
	auto uav_out = m_textures->texture_reflection_color.GetUnorderedAccessView();
	m_context->CSSetUnorderedAccessViews(0, 1, uav_out.GetAddressOf(), nullptr);

	const auto width = m_textures->texture_reflection_color.GetWidth();
	const auto height = m_textures->texture_reflection_color.GetHeight();

	m_context->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

	// unbind
	ID3D11UnorderedAccessView* null_uav = nullptr;
	m_context->CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);
}

void PassSSR::DrawBlur()
{
	m_context->GenerateMips(m_textures->texture_reflection_color.GetShaderResourceView().Get());
	return;
	// TODO manual mip map generation
}