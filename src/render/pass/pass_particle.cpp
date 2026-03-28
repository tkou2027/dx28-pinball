#include "pass_particle.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_scene.h"
#include "render/render_states.h"
#include "render/resource/buffer.h"
#include "render/particle/particle_system.h"
#include "render/util/dx_trace.h"

using namespace Microsoft::WRL;

namespace
{
	struct VertexSprite
	{
		float position[3];
		float uv[2];
	};
	struct PlaneCommon
	{
		DirectX::XMFLOAT2 uv_offset;
		DirectX::XMFLOAT2 uv_scale;
		DirectX::XMFLOAT2 size;
		float _padding_1;
		float _padding_2;
	};
}

void PassParticle::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	PassBase::Initialize(pDevice, pContext);

	// Use sprite input layout (position + uv)
	m_vs = Shader::CreateShaderVertex(m_device, L"vertex_texture_particle.cso", Shader::InputLayoutType::APPEND_POS_TEX);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_texture_particle.cso");

	m_cb_plane_common = Buffer::CreateConstantBuffer(m_device, sizeof(PlaneCommon));

	// create quad VB (4 vertices: XY in local quad space, UV)
	VertexSprite verts[4] =
	{
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } }
	};
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(verts);
	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = verts;
	HR(m_device->CreateBuffer(&bd, &initData, m_vb_quad.ReleaseAndGetAddressOf()));

	// index buffer for two triangles (6 indices)
	uint16_t indices[6] = { 0, 1, 2, 2, 1, 3 };
	D3D11_BUFFER_DESC ibd{};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = sizeof(indices);
	D3D11_SUBRESOURCE_DATA iinit{};
	iinit.pSysMem = indices;
	HR(m_device->CreateBuffer(&ibd, &iinit, m_ib_quad.ReleaseAndGetAddressOf()));
}

void PassParticle::Draw()
{
	SetInfoPerDraw();
	const auto& particle_system = g_global_context.m_render_system->GetParticleSystem();
	const auto emitter_render_data = particle_system.GetParticleManager().GetEmitterRenderData();
	for (const auto emitter : emitter_render_data.emitters)
	{
		DrawParticles(*emitter);
	}
	ResetPerDraw();
}

void PassParticle::SetInfoPerDraw()
{
	// set shaders & input layout
	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto& render_resource = GetRenderResource();
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->VSSetConstantBuffers(2, 1, m_cb_plane_common.GetAddressOf());

	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_none.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);

	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
}

void PassParticle::DrawParticles(const EmitterData& item)
{
	const EmitterDesc& item_config = item.desc;

	// particle structured buffer SRV
	auto& model_loader = GetModelLoader();
	ID3D11ShaderResourceView* srv_particle = item.buffer_particles.buffer_srv.Get();
	m_context->VSSetShaderResources(1, 1, &srv_particle);

	// plane common buffer (rotation + size)
	{
		PlaneCommon cb{};
		cb.uv_offset = item_config.texture_uv.uv_offset.ToXMFLOAT2();
		cb.uv_scale = item_config.texture_uv.uv_size.ToXMFLOAT2();
		cb.size = item_config.local_mesh_size.ToXMFLOAT2();
		m_context->UpdateSubresource(m_cb_plane_common.Get(), 0, nullptr, &cb, 0, 0);
	}

	// particle texture (pixel shader)
	auto& texture_loader = GetTextureLoader();
	// int texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/background.png"); // replace with your particle texture
	auto srv_tex = texture_loader.GetTexture(item_config.texture_id);
	m_context->PSSetShaderResources(0, 1, srv_tex.GetAddressOf());
	// if (item_config.local_texture_id >= 0)
	// {
	auto srv_local_tex = texture_loader.GetTexture(item_config.local_texture_id);
	m_context->PSSetShaderResources(1, 1, srv_local_tex.GetAddressOf());
	// }

	// bind quad VB / IB
	UINT stride = sizeof(VertexSprite);
	UINT offset = 0;
	ID3D11Buffer* vbs[] = { m_vb_quad.Get() };
	m_context->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
	m_context->IASetIndexBuffer(m_ib_quad.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Draw indexed instanced: 6 indices per instance
	UINT num_instances = item_config.num_width * item_config.num_height;
	m_context->DrawIndexedInstanced(6, num_instances, 0, 0, 0);
}

void PassParticle::ResetPerDraw()
{
	ID3D11ShaderResourceView* null_srv = nullptr;
	m_context->VSSetShaderResources(1, 1, &null_srv);
	m_context->PSSetShaderResources(0, 1, &null_srv);
}
