#include "pass_composite.h"
#include "render/render_states.h"

void PassComposite::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	// shaders
	m_vertex_shader = Shader::CreateShaderVertex(m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC);
	
	m_ps_add = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_add.cso");
	m_ps_add_reflection = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_add_reflection.cso");
	m_ps_tone_mapping = Shader::CreateShaderPixel(m_device, L"pixel_fullscreen_tone_mapping.cso");
}

void PassComposite::Draw()
{
	// m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	//m_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
	//m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());

	//// texture
	//const auto& render_states = GetRenderStates();
	//m_context->RSSetState(render_states.m_rs_cull_back.Get());
	//m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	//m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	//m_context->PSSetShaderResources(0, 1, &m_srv_base_color);
	//m_context->PSSetShaderResources(1, 1, &m_srv_reflection);
	//m_context->PSSetShaderResources(2, 1, &m_srv_emission);
	//// draw
	//m_context->Draw(3, 0); // draw fullscreen triangle
}

//void PassComposite::SetInputTextureBase(ID3D11ShaderResourceView* srv)
//{
//	m_srv_base_color = srv;
//}
//
//void PassComposite::SetInputTextureReflection(ID3D11ShaderResourceView* srv)
//{
//	m_srv_reflection = srv;
//}
//
//void PassComposite::SetInputTextureEmmision(ID3D11ShaderResourceView* srv)
//{
//	m_srv_emission = srv;
//}

void PassComposite::DrawAddReflection(
	ID3D11ShaderResourceView* srv_base,
	ID3D11ShaderResourceView* srv_reflection,
	ID3D11ShaderResourceView* srv_g_buffer_material
)
{
	m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps_add_reflection.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// texture
	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, 1, &srv_base);
	m_context->PSSetShaderResources(1, 1, &srv_reflection);
	m_context->PSSetShaderResources(2, 1, &srv_g_buffer_material);
	// draw
	m_context->Draw(3, 0); // draw fullscreen triangle
	// cleanup srvs
	ID3D11ShaderResourceView* null_srvs[3]{ nullptr, nullptr };
	m_context->PSSetShaderResources(0, 3, null_srvs);
}

void PassComposite::DrawAdd(ID3D11ShaderResourceView* srv_a, ID3D11ShaderResourceView* srv_b)
{
	m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps_add.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// texture
	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, 1, &srv_a);
	m_context->PSSetShaderResources(1, 1, &srv_b);
	// draw
	m_context->Draw(3, 0); // draw fullscreen triangle
	// cleanup srvs
	ID3D11ShaderResourceView* null_srvs[2]{ nullptr, nullptr };
	m_context->PSSetShaderResources(0, 2, null_srvs);
}

void PassComposite::DrawToneMapping(ID3D11ShaderResourceView* srv)
{
	m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps_tone_mapping.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// texture
	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, 1, &srv);
	// draw
	m_context->Draw(3, 0);
	// cleanup srvs
	ID3D11ShaderResourceView* null_srv{ nullptr };
	m_context->PSSetShaderResources(0, 1, &null_srv);
}
