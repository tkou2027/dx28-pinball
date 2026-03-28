#include "pass_particle_cloth.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_scene.h"
#include "render/render_states.h"
#include "render/resource/buffer.h"
#include "render/util/dx_trace.h"
using namespace Microsoft::WRL;

namespace
{
	struct ParticleConfig
	{
		UINT width;
		UINT height;
		float _padding_0;
		float _padding_1;
	};
}

void PassParticleCloth::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	PassBase::Initialize(pDevice, pContext);

	// Use sprite input layout (position + uv)
	m_vs = Shader::CreateShaderVertex(m_device, L"vertex_textured_particle_cloth.cso", Shader::InputLayoutType::APPEND_POS_TEX);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_textured_particle_cloth.cso");
	m_cb = Buffer::CreateConstantBuffer(m_device, sizeof(ParticleConfig));

	//// create quad VB (4 vertices: XY in local quad space, UV)
	//VertexSprite verts[4] =
	//{
	//	{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
	//	{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } },
	//	{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
	//	{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } }
	//};
	//D3D11_BUFFER_DESC bd{};
	//bd.Usage = D3D11_USAGE_IMMUTABLE;
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.ByteWidth = sizeof(verts);
	//D3D11_SUBRESOURCE_DATA initData{};
	//initData.pSysMem = verts;
	//HR(m_device->CreateBuffer(&bd, &initData, m_vb_quad.ReleaseAndGetAddressOf()));

	//// index buffer for two triangles (6 indices)
	//uint16_t indices[6] = { 0, 1, 2, 2, 1, 3 };
	//D3D11_BUFFER_DESC ibd{};
	//ibd.Usage = D3D11_USAGE_IMMUTABLE;
	//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//ibd.ByteWidth = sizeof(indices);
	//D3D11_SUBRESOURCE_DATA iinit{};
	//iinit.pSysMem = indices;
	//HR(m_device->CreateBuffer(&ibd, &iinit, m_ib_quad.ReleaseAndGetAddressOf()));
}

void PassParticleCloth::Draw()
{
	SetInfoPerDraw();
	const auto& render_scene = g_global_context.m_render_system->GetRenderScene();
	//for (const auto& item : render_scene.m_particles)
	//{
	//	DrawItem(item);
	//}
	// TODO
	ResetPerDraw();
}

void PassParticleCloth::SetInfoPerDraw()
{
	// set shaders & input layout
	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto& render_resource = GetRenderResource();
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->VSSetConstantBuffers(2, 1, m_cb.GetAddressOf());

	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_none.Get()); // TODO
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);

	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
}

void PassParticleCloth::DrawItem(const ParticleEmitter& item)
{
	//EmitterDesc item_config = item.GetConfig();
	//auto& model_loader = GetModelLoader();
	//// index buffer
	//const auto& model_data = model_loader.GetModel(item_config.model_id);
	//if (model_data.meshes.empty())
	//{
	//	return;
	//}
	//const auto& mesh_data = model_data.meshes[0];
	//m_context->IASetIndexBuffer(
	//	mesh_data.m_indices.Get(),
	//	mesh_data.m_index_count > 65535 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);

	//// particle structured buffer SRV
	//auto& buffer_view = model_loader.GetBufferView(item_config.buffer_view_id);
	//ID3D11ShaderResourceView* srv_particle = buffer_view.buffer_srv.Get();
	//m_context->VSSetShaderResources(1, 1, &srv_particle);

	//// constant buffer
	//{
	//	ParticleConfig cb{};
	//	cb.width = item_config.num_width;
	//	cb.height = item_config.num_width;
	//	m_context->UpdateSubresource(m_cb.Get(), 0, nullptr, &cb, 0, 0);
	//}

	//// particle texture (pixel shader)
	//auto& texture_loader = GetTextureLoader();
	//// int texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/background.png");
	//auto srv_tex = texture_loader.GetTexture(item_config.texture_id);
	//m_context->PSSetShaderResources(0, 1, srv_tex.GetAddressOf());

	//m_context->DrawIndexed(static_cast<UINT>(mesh_data.m_index_count), 0, 0);
}

void PassParticleCloth::ResetPerDraw()
{
	ID3D11ShaderResourceView* null_srv = nullptr;
	m_context->VSSetShaderResources(1, 1, &null_srv);
	m_context->PSSetShaderResources(0, 1, &null_srv);
}
