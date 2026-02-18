#include "pass_postprocess.h"
#include "render/render_states.h"

void PassPostprocess::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	// shaders
	m_vertex_shader = Shader::CreateShaderVertex(m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC);
	m_pixel_shader = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_copy.cso");
}

void PassPostprocess::Draw()
{
	m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());

	// texture
	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, 1, &m_srv_input);

	// draw
	m_context->Draw(3, 0); // draw fullscreen triangle
}

void PassPostprocess::SetInputResource(ID3D11ShaderResourceView* srv)
{
	m_srv_input = srv;
}
