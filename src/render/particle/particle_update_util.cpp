#include "particle_update_util.h"
#include "render/resource/buffer.h"
#include "render/resource/shader.h"
#include "render/render_states.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"

using namespace DirectX;

namespace
{
	struct ConstanceBufferInit
	{
		XMFLOAT4X4 matrix_model;
		UINT width;
		UINT height;
		UINT shape_type;
		float _padding_1;
	};

	struct ConstantBufferUpdate
	{
		float dt;
		XMFLOAT3 gravity;

		float lifetime;
		XMFLOAT3 world_center;

		XMFLOAT2 world_scale;

		UINT num_particles;
		float _padding_1;

		XMFLOAT2 texture_uv_offset;
		XMFLOAT2 texture_uv_scale;
	};
}

void ParticleUpdateUtil::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	// init
	m_cs_init = Shader::CreateShaderCompute(m_device, L"compute_particle_init_texture_mesh.cso");
	m_cb_init = Buffer::CreateConstantBuffer(m_device, sizeof(ConstanceBufferInit));
	// burst init
	m_cs_burst_init = Shader::CreateShaderCompute(m_device, L"compute_particle_burst_init_texture_mesh.cso");

	// update
	m_cs_update_crush = Shader::CreateShaderCompute(m_device, L"compute_particle_burst_update_texture_mesh.cso");
	m_cb_update_crush = Buffer::CreateConstantBuffer(m_device, sizeof(ConstantBufferUpdate));
}

void ParticleUpdateUtil::InitializeEmitterParticles(const EmitterData& emitter) const
{
	ConstanceBufferInit cb{};
	XMMATRIX world = XMLoadFloat4x4(&emitter.transform);
	XMStoreFloat4x4(&cb.matrix_model, XMMatrixTranspose(world));
	cb.shape_type = static_cast<UINT>(emitter.desc.shape);
	cb.width = emitter.desc.num_width;
	cb.height = emitter.desc.num_height;
	m_context->UpdateSubresource(m_cb_init.Get(), 0, nullptr, &cb, 0, 0);

	m_context->CSSetShader(m_cs_init.Get(), nullptr, 0);
	m_context->CSSetUnorderedAccessViews(0, 1, emitter.buffer_particles.buffer_uav.GetAddressOf(), nullptr);
	m_context->CSSetConstantBuffers(0, 1, m_cb_init.GetAddressOf());

	m_context->Dispatch(
		(emitter.desc.num_width + 15) / 16,
		(emitter.desc.num_height + 15) / 16,
		1
	);

	ResetComputeShader();
}

void ParticleUpdateUtil::InitializeEmitterBurstUpdate(const EmitterUpdateData& update_data, const EmitterData& emitter) const
{
	//const UINT num_particles = emitter.desc.num_width * emitter.desc.num_height;
	//ConstantBufferUpdate cb
	//{
	//	update_data.delta_time,
	//	{ 0.0f, -98.0f, 0.0f },
	//	num_particles
	//};
	//m_context->UpdateSubresource(m_cb_update_crush.Get(), 0, nullptr, &cb, 0, 0);

	//m_context->CSSetShader(m_cs_update_crush.Get(), nullptr, 0);
	//m_context->CSSetUnorderedAccessViews(0, 1, emitter.buffer_particles.buffer_uav.GetAddressOf(), nullptr);
	//m_context->CSSetConstantBuffers(0, 1, m_cb_update_crush.GetAddressOf());

	//const auto& render_states = g_global_context.m_render_system->GetRenderStates();
	//auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	//const auto texture_id_noise = texture_loader.GetOrLoadTextureFromFile("asset/texture/noise/perlin1.png");
	//const auto srv_noise = texture_loader.GetTexture(texture_id_noise);
	//m_context->CSSetShaderResources(0, 1, srv_noise.GetAddressOf());
	//m_context->CSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());

	//m_context->Dispatch((num_particles + 255) / 256, 1, 1);

	//ResetComputeShader();
}

void ParticleUpdateUtil::UpdateEmitterBurstUpdate(
	const EmitterUpdateData& update_data, const EmitterData& emitter) const
{
	const UINT num_particles = emitter.desc.num_width * emitter.desc.num_height;
	const float world_scale_x = 1.0f / (emitter.desc.local_mesh_size.x * emitter.desc.num_width);
	const float world_scale_y = 1.0f / (emitter.desc.local_mesh_size.y * emitter.desc.num_height);
	ConstantBufferUpdate cb{};

	cb.dt = update_data.delta_time;
	cb.gravity = emitter.desc.gravity.ToXMFLOAT3();
	cb.lifetime = emitter.desc.lifetime;
	cb.num_particles = num_particles;
	cb.world_scale = XMFLOAT2(world_scale_x, world_scale_y);
	cb.world_center = XMFLOAT3(emitter.transform.m[3][0], emitter.transform.m[3][1], emitter.transform.m[3][2]); // translation part of the transform

	cb.texture_uv_offset = emitter.desc.texture_uv.uv_offset.ToXMFLOAT2();
	cb.texture_uv_scale = emitter.desc.texture_uv.uv_size.ToXMFLOAT2();

	m_context->UpdateSubresource(m_cb_update_crush.Get(), 0, nullptr, &cb, 0, 0);

	m_context->CSSetShader(m_cs_update_crush.Get(), nullptr, 0);
	m_context->CSSetUnorderedAccessViews(0, 1, emitter.buffer_particles.buffer_uav.GetAddressOf(), nullptr);
	m_context->CSSetConstantBuffers(0, 1, m_cb_update_crush.GetAddressOf());

	const auto& render_states = g_global_context.m_render_system->GetRenderStates();
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	const auto texture_id_noise = texture_loader.GetOrLoadTextureFromFile("asset/texture/noise/cyclone.png");
	const auto srv_noise = texture_loader.GetTexture(texture_id_noise);
	m_context->CSSetShaderResources(0, 1, srv_noise.GetAddressOf());
	m_context->CSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());

	m_context->Dispatch((num_particles + 255) / 256, 1, 1);

	ResetComputeShader();
}

void ParticleUpdateUtil::ResetComputeShader() const
{
	ID3D11UnorderedAccessView* null_uav = nullptr;
	m_context->CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);
	m_context->CSSetConstantBuffers(0, 0, nullptr);
	m_context->CSSetShader(nullptr, nullptr, 0);
}
