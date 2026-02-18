#include "texture_particle.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/particle/particle_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/resource/buffer.h"
#include "texture_particle_item.h"

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
		UINT num_particles;
		float _padding_1;
		float _padding_2;
		float _padding_3;
	};
}

void TextureParticleManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	m_cb_init = Buffer::CreateConstantBuffer(m_device, sizeof(ConstanceBufferInit));
	m_cb_update_crush = Buffer::CreateConstantBuffer(m_device, sizeof(ConstantBufferUpdate));
	m_cs_init = Shader::CreateShaderCompute(m_device, L"texture_particle_init.cso");
	m_cs_update_crush = Shader::CreateShaderCompute(m_device, L"texture_particle_update_crush.cso");
}

int TextureParticleManager::InitializeOrResetParticle(const TextureParticleConfig& config, const XMFLOAT4X4& model_matrix)
{
	auto& model_loader = GetModelLoader();
	int buffer_view_id = config.buffer_view_id;
	if (buffer_view_id < 0)
	{
		BufferView buffer_view{};
		UINT num_particles = config.num_width * config.num_height;
		Buffer::CreateStructuredBuffer(
			m_device, sizeof(TextureParticleData), num_particles,
			buffer_view.buffer, buffer_view.buffer_srv, buffer_view.buffer_uav
		);
		buffer_view_id = model_loader.AddBufferView(buffer_view);
	}
	auto& buffer_view = model_loader.GetBufferView(buffer_view_id);
	
	ConstanceBufferInit cb{};
	XMMATRIX world = XMLoadFloat4x4(&model_matrix);
	XMStoreFloat4x4(&cb.matrix_model, XMMatrixTranspose(world));
	cb.shape_type = static_cast<UINT>(config.shape);
	cb.width = config.num_width;
	cb.height = config.num_height;
	m_context->UpdateSubresource(m_cb_init.Get(), 0, nullptr, &cb, 0, 0);

	m_context->CSSetShader(m_cs_init.Get(), nullptr, 0);
	m_context->CSSetUnorderedAccessViews(0, 1, buffer_view.buffer_uav.GetAddressOf(), nullptr);
	m_context->CSSetConstantBuffers(0, 1, m_cb_init.GetAddressOf());

	m_context->Dispatch(
		(config.num_width + 15) / 16,
		(config.num_height + 15) / 16,
		1
	);

	ResetComputeShader();

	return buffer_view_id;
}

void TextureParticleManager::UpdateParticles(const TextureParticleConfig& config, float delta_time)
{
	assert(config.buffer_view_id >= 0);
	auto& model_loader = GetModelLoader();
	auto& buffer_view = model_loader.GetBufferView(config.buffer_view_id);

	const UINT num_particles = config.num_width * config.num_height;
	ConstantBufferUpdate cb
	{
		delta_time,
		{ 0.0f, -98.0f, 0.0f },
		config.num_width * config.num_height
	};
	m_context->UpdateSubresource(m_cb_update_crush.Get(), 0, nullptr, &cb, 0, 0);

	m_context->CSSetShader(m_cs_update_crush.Get(), nullptr, 0);
	m_context->CSSetUnorderedAccessViews(0, 1, buffer_view.buffer_uav.GetAddressOf(), nullptr);
	m_context->CSSetConstantBuffers(0, 1, m_cb_update_crush.GetAddressOf());

	const auto& render_states = g_global_context.m_render_system->GetRenderStates();
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	const auto texture_id_noise = texture_loader.GetOrLoadTextureFromFile("asset/texture/noise/perlin1.png");
	const auto srv_noise = texture_loader.GetTexture(texture_id_noise);
	m_context->CSSetShaderResources(0, 1, srv_noise.GetAddressOf());
	m_context->CSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());

	m_context->Dispatch((num_particles + 255) / 256, 1, 1);

	ResetComputeShader();
}

void TextureParticleManager::ResetComputeShader()
{
	ID3D11UnorderedAccessView* null_uav = nullptr;
	m_context->CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);
	m_context->CSSetConstantBuffers(0, 0, nullptr);
	m_context->CSSetShader(nullptr, nullptr, 0);
}

ModelLoader& TextureParticleManager::GetModelLoader() const
{
	return g_global_context.m_render_system->GetRenderResource().GetModelLoader();
}
