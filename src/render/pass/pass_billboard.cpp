#include "pass_billboard.h"
#include "render/render_resource.h"
#include "render/render_scene.h"
#include "render/render_states.h"

void PassBillboard::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	m_vs_layout = Shader::CreateShaderVertex(m_device, L"vertex_billboard.cso", Shader::InputLayoutType::MESH_STATIC);
	m_gs = Shader::CreateShaderGeometry(m_device, L"geo_billboard.cso");
	m_ps = Shader::CreateShaderPixel(m_device, L"sprite_pixel.cso");
}
void PassBillboard::Draw()
{
	auto& billboard_data = GetRenderScene().m_billboards;
	if (billboard_data.GetBatchCommands().empty())
	{
		return;
	}

	SetInfoPerDraw();

	for (const auto& cmd : billboard_data.GetBatchCommands())
	{
		auto texture = GetTextureLoader().GetTexture(cmd.texture_id);
		m_context->PSSetShaderResources(0, 1, texture.GetAddressOf());

		// TODO: other per batch info

		m_context->DrawInstanced(1, cmd.instance_count, 0, cmd.start_index);
	}

	m_context->GSSetShader(nullptr, nullptr, 0);
}

void PassBillboard::SetInfoPerDraw()
{
	m_context->VSSetShader(m_vs_layout.vertex_shader.Get(), nullptr, 0);
	m_context->GSSetShader(m_gs.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	// TODO: input layout
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// geometry
	m_context->GSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->GSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	auto& batch_data_srv = GetRenderScene().m_billboards.m_srv;
	m_context->GSSetShaderResources(0, 1, batch_data_srv.GetAddressOf());
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());

	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_read.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_alpha.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}
