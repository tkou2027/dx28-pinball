#include "pass_blur_dual.h"
#include "render/resource/buffer.h"

#include "render/render_states.h"

namespace
{
	struct DownSampleConfig
	{
		DirectX::XMFLOAT2 src_resolution;
		float _padding_1{};
		float _padding_2{};
	};

	struct UpSampleConfig
	{
		float filter_radius;
		float _padding_1{};
		float _padding_2{};
		float _padding_3{};
	};
}

void PassBlurDual::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	m_vs = Shader::CreateShaderVertex(
		m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC);
	m_ps_down = Shader::CreateShaderPixel(m_device, L"pixel_blur_dual_down.cso");
	m_ps_up = Shader::CreateShaderPixel(m_device, L"pixel_blur_dual_up.cso");
	m_cb_down = CreateConstantBuffer(sizeof(DownSampleConfig));
	m_cb_up = CreateConstantBuffer(sizeof(UpSampleConfig));
}

void PassBlurDual::CreateTextures(uint32_t width, uint32_t height, InternalTextures& out_context)
{
	out_context.initial_width = width;
	out_context.initial_height = height;
	uint32_t mip_width = width;
	uint32_t mip_height = height;
	auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET_HDR;
	for (int i = 0; i < MAX_MIP_LEVELS; i++)
	{
		mip_width /= 2;
		mip_height /= 2;
		if (mip_width < 1 || mip_height < 1)
		{
			break;
		}
		out_context.mip_levels++;

		desc.Width = mip_width;
		desc.Height = mip_height;
		out_context.mip_textures[i].InitializeRenderTarget2D(m_device, desc);
	}
}

void PassBlurDual::SetInputResource(const InternalTextures* textures, ID3D11ShaderResourceView* srv_input)
{
	m_textures = textures;
	m_srv_input = srv_input;
}

void PassBlurDual::Draw()
{
	assert(m_textures && m_srv_input);
	SetInfoPerDraw();
	DrawDownSample();
	DrawUpSample();
}

void PassBlurDual::SetInfoPerDraw()
{
	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	// avoid frame buffer still bind
	UnbindResources();
}

void PassBlurDual::DrawDownSample()
{
	const auto& render_states = GetRenderStates();
	m_context->PSSetShader(m_ps_down.Get(), nullptr, 0);
	m_context->PSSetConstantBuffers(0, 1, m_cb_down.GetAddressOf());
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	float sample_width = static_cast<float>(m_textures->initial_width);
	float sample_height = static_cast<float>(m_textures->initial_height);
	for (int i = 0; i < m_textures->mip_levels; i++)
	{
		const auto& target_texture = m_textures->mip_textures[i];
		RenderTarget rt{};
		rt.Initialize(target_texture.GetWidth(), target_texture.GetHeight());
		rt.AddRenderTarget(target_texture.GetRenderTargetView());
		// constant buffer
		{
			DownSampleConfig cb{};
			cb.src_resolution = DirectX::XMFLOAT2(sample_width, sample_height);
			m_context->UpdateSubresource(m_cb_down.Get(), 0, nullptr, &cb, 0, 0);
		}
		// input texture
		ID3D11ShaderResourceView* srv = i == 0 ? m_srv_input : m_textures->mip_textures[i - 1].GetShaderResourceView().Get();
		m_context->PSSetShaderResources(0, 1, &srv);

		// draw
		rt.Bind(m_context);
		m_context->Draw(3, 0);

		// update resolution
		sample_width = static_cast<float>(target_texture.GetWidth());
		sample_height = static_cast<float>(target_texture.GetHeight());
		UnbindResources();
	}
}

void PassBlurDual::DrawUpSample()
{
	const auto& render_states = GetRenderStates();
	m_context->PSSetShader(m_ps_up.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetConstantBuffers(0, 1, m_cb_up.GetAddressOf());
	{
		UpSampleConfig cb{};
		cb.filter_radius = 0.005f; // TODO: parameterize
		m_context->UpdateSubresource(m_cb_up.Get(), 0, nullptr, &cb, 0, 0);
	}
	// additive blending
	m_context->OMSetBlendState(render_states.m_bs_add.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	for (int i = m_textures->mip_levels - 1; i > 0; i--)
	{
		const auto& source_texture = m_textures->mip_textures[i];
		const auto& target_texture = m_textures->mip_textures[i - 1];

		// input texture
		m_context->PSSetShaderResources(0, 1, source_texture.GetShaderResourceView().GetAddressOf());

		RenderTarget rt{};
		rt.Initialize(target_texture.GetWidth(), target_texture.GetHeight());
		rt.AddRenderTarget(target_texture.GetRenderTargetView());
		rt.Bind(m_context);

		m_context->Draw(3, 0);
		// unbind resource
		UnbindResources();
	}
}

void PassBlurDual::UnbindResources()
{
	// srv
	ID3D11ShaderResourceView* null_srv[1] = { nullptr };
	m_context->PSSetShaderResources(0, 1, null_srv);
	// rtv
	// avoid frame buffer / emission still binded
	ID3D11RenderTargetView* null_rtvs[8] = { nullptr };
	m_context->OMSetRenderTargets(8, null_rtvs, nullptr);
}
