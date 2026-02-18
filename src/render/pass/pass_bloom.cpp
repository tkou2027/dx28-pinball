#include "pass_bloom.h"
#include "render/render_states.h"

namespace
{
	struct GaussianConfig
	{
		float sample_offset;
		BOOL horizontal;
		float _padding_1{ 0.0f };
		float _padding_2{ 0.0f };
	};
}

void PassBloom::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);

	m_vs = Shader::CreateShaderVertex(
		m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC);
	m_ps_blur = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_gaussian.cso");
	m_ps_add = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_add.cso");

	m_rt_blur.Initialize(device, context, GetScreenWidth(), GetScreenHeight());
	m_rt_blur.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET_HDR); // vertical res
	m_rt_blur.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET_HDR); // horizontal res

	m_rt_add.Initialize(device, context, GetScreenWidth(), GetScreenHeight());
	m_rt_add.AddTextureColor(TextureResource2D::DESC_PREST_RENDER_TARGET_HDR);

	m_cb_blur = CreateConstantBuffer(sizeof(GaussianConfig));
}

void PassBloom::Draw()
{
	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps_blur.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto& render_states = GetRenderStates();
	m_context->PSSetConstantBuffers(0, 1, m_cb_blur.GetAddressOf());
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());

	m_context->RSSetState(render_states.m_rs_cull_none.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	bool init_run{ true };
	int runs = 5 * 2;
	bool horizontal{ true };
	GaussianConfig config{};
	float offset_x = 1.0f / m_rt_blur.GetWidth();
	float offset_y = 1.0f / m_rt_blur.GetHeight();

	float black[4] = { 0.0f ,0.0f, 0.0f, 1.0f };
	m_rt_blur.ClearColor(black);

	for (int i = 0; i < runs; i++)
	{
		
		//auto srv_in = init_run ? &m_srv_blur :
		//	m_rt_blur.GetTextureColor<TextureResource2D>(static_cast<size_t>(!horizontal))->GetShaderResourceView().GetAddressOf();
		//// input texture
		//m_context->PSSetShaderResources(0, 1, srv_in);
		ID3D11RenderTargetView* notarget[1] = {nullptr};
		m_context->OMSetRenderTargets(
			1, notarget,
			nullptr);
		if (init_run)
		{
			m_context->PSSetShaderResources(0, 1, &m_srv_blur);
		}
		else
		{
			auto srv = m_rt_blur.GetTextureColor<TextureResource2D>(static_cast<size_t>(!horizontal))->GetShaderResourceView();
			m_context->PSSetShaderResources(0, 1, srv.GetAddressOf());
		}

		// output texture
		m_rt_blur.BindRenderTargetNo(static_cast<size_t>(horizontal));

		// set constant buffer
		config.horizontal = horizontal;
		config.sample_offset = horizontal ? offset_x : offset_y;
		m_context->UpdateSubresource(m_cb_blur.Get(), 0, nullptr, &config, 0, 0);

		// draw
		m_context->Draw(3, 0); // draw fullscreen triangle
		horizontal = !horizontal;
		init_run = false;
	}
}

void PassBloom::DrawOutput()
{

	float black[4] = { 0.0f ,0.0f, 0.0f, 0.0f };
	m_rt_add.ClearColor(black);
	m_rt_add.Bind();
	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps_add.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs.input_layout.Get());

	const auto& render_states = GetRenderStates();
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());

	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);

	m_context->PSSetShaderResources(0, 1, &m_srv_add);
	m_context->PSSetShaderResources(1, 1,
		m_rt_blur.GetTextureColor<TextureResource2D>(0)->GetShaderResourceView().GetAddressOf());

	m_context->Draw(3, 0);
}
