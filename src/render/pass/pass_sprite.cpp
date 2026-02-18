#include "pass_sprite.h"
#include "render/dx_trace.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include "render/resource/texture_loader.h"
#include "config/constant.h"

using namespace DirectX;
namespace
{
	struct Vertex
	{
		XMFLOAT3 position; // 頂点座標
		XMFLOAT2 texcoord; // テキスチャ座標
		XMFLOAT4 color;
	};

	static constexpr int VERTEX_NUM{ 4 };
}

void PassSprite::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	// shaders
	m_vertex_shader = Shader::CreateShaderVertex(m_device, L"sprite_vertex.cso", Shader::InputLayoutType::SPRITE_DEFAULT);
	m_pixel_shader = Shader::CreateShaderPixel(m_device, L"sprite_pixel.cso");

	// 頂点バッファ生成
	{
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Vertex) * VERTEX_NUM;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(m_device->CreateBuffer(&bd, NULL, m_vertex_buffer.GetAddressOf()));
	}
	{
		D3D11_BUFFER_DESC bd{};
		bd.ByteWidth = sizeof(XMMATRIX);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		HR(m_device->CreateBuffer(&bd, nullptr, m_constant_buffer.GetAddressOf()));
	}
}

void PassSprite::Draw()
{
	Begin();
	const auto& renderable_manager = GetRenderScene().GetRenderablesManager();
	const auto& sprites = renderable_manager.m_sprites;
	const auto& sprite_indices = renderable_manager.GetRenderablesOfLayer(m_curr_render_layer).indices_sprite;
	for (int index : sprite_indices)
	{
		auto& sprite = sprites[index];
		DrawSprite(sprite);
	}
}

void PassSprite::SetRenderLayer(CameraRenderLayer layer)
{
	m_curr_render_layer = layer;
}

void PassSprite::Begin()
{
	m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, m_vertex_buffer.GetAddressOf(), &stride, &offset);
	auto& render_resource = GetRenderResource();
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection_sprite.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, m_constant_buffer.GetAddressOf());

	const auto& render_states = GetRenderStates();
	m_context->PSSetSamplers(0, 1, render_states.m_ss_point_wrap.GetAddressOf());

	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
}

void PassSprite::DrawSprite(const SpriteOutput& sprite)
{
	// 頂点バッファにデータを設定
	D3D11_MAPPED_SUBRESOURCE msr;
	m_context->Map(m_vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	Vertex* v = (Vertex*)msr.pData;
	v[0].position = { -0.5, -0.5, 0.0f };
	v[1].position = { 0.5, -0.5, 0.0f };
	v[2].position = { -0.5,  0.5, 0.0f };
	v[3].position = { 0.5,  0.5, 0.0f };

	v[0].texcoord = { sprite.uv_offset.x,                    sprite.uv_offset.y };
	v[1].texcoord = { sprite.uv_offset.x + sprite.uv_size.x, sprite.uv_offset.y };
	v[2].texcoord = { sprite.uv_offset.x,                    sprite.uv_offset.y + sprite.uv_size.y };
	v[3].texcoord = { sprite.uv_offset.x + sprite.uv_size.x, sprite.uv_offset.y + sprite.uv_size.y };

	for (int i = 0; i < 4; i++)
	{
		v[i].color = { sprite.color.x, sprite.color.y, sprite.color.z, sprite.color.w };
	}
	m_context->Unmap(m_vertex_buffer.Get(), 0);

	// matrix
	const auto& transform = sprite.transform;
	XMMATRIX matrix = XMMatrixIdentity();
	matrix *= XMMatrixScaling(sprite.size.x * transform.scale.x, sprite.size.y * transform.scale.y, 1.0f);
	matrix *= XMMatrixRotationZ(transform.rotation);
	matrix *= XMMatrixTranslation(sprite.offset.x + transform.position.x, sprite.offset.y + transform.position.y, 0);
	// matrix *= XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f); // TODO
	XMFLOAT4X4 transpose;
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));
	m_context->UpdateSubresource(m_constant_buffer.Get(), 0, nullptr, &transpose, 0, 0);

	// テクスチャ設定
	const auto& texture_loader = GetTextureLoader();
	auto texture = texture_loader.GetTexture(sprite.texture_id);
	m_context->PSSetShaderResources(0, 1, texture.GetAddressOf());

	const auto& render_states = GetRenderStates();
	float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (sprite.blend_add)
	{
		m_context->OMSetBlendState(render_states.m_bs_add_alpha.Get(), blend_factor, 0xffffffff);
	}
	else
	{
		m_context->OMSetBlendState(render_states.m_bs_alpha.Get(), blend_factor, 0xffffffff);
	}

	// ポリゴン描画
	m_context->Draw(4, 0);
}
