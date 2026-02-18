#include "render_attachment.h"
template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

void RenderAttachment::Initialize(ID3D11Device* device, ID3D11DeviceContext* context,
	int width, int height)
{
	m_device = device;
	m_context = context;
	m_texture_color.clear();
	Resize(width, height);
}

void RenderAttachment::SetViewPort()
{
	// ビューポートの設定
	m_view_port.Width = static_cast<float>(m_width);
	m_view_port.Height = static_cast<float>(m_height);
	m_view_port.MinDepth = 0.0f;
	m_view_port.MaxDepth = 1.0f;
	m_view_port.TopLeftX = 0;
	m_view_port.TopLeftY = 0;
}

void RenderAttachment::Resize(int width, int height)
{
	m_width = width;
	m_height = height;
	for (auto& texture_color : m_texture_color)
	{
		texture_color->Resize(m_device, width, height);
	}
	if (m_texture_depth_stencil)
	{
		m_texture_depth_stencil->Resize(m_device, width, height);
	}
	SetViewPort();
}

size_t RenderAttachment::AddTextureColorFromSwapChain(IDXGISwapChain* swap_chain)
{
	auto texture_color = std::make_shared<TextureResource2D>();
	texture_color->InitializeRenderTarget2DFromSwapChain(m_device, swap_chain);
	const auto index = m_texture_color.size();
	m_texture_color.push_back(texture_color);
	return index;
}

size_t RenderAttachment::AddTextureColor(const D3D11_TEXTURE2D_DESC& texture_desc)
{
	auto texture_desc_sized{ texture_desc };
	texture_desc_sized.Width = m_width;
	texture_desc_sized.Height = m_height;
	auto texture_color = std::make_shared<TextureResource2D>();
	texture_color->InitializeRenderTarget2D(m_device, texture_desc_sized);
	const auto index = m_texture_color.size();
	m_texture_color.push_back(texture_color);
	return index;
}

size_t RenderAttachment::AddTextureColor(std::shared_ptr<TextureResource2D> texture)
{;
	const auto index = m_texture_color.size();
	m_texture_color.push_back(texture);
	return index;
}

void RenderAttachment::SetTextureDepthStencil(const D3D11_TEXTURE2D_DESC& texture_desc)
{
	auto texture_desc_sized{ texture_desc };
	texture_desc_sized.Width = m_width;
	texture_desc_sized.Height = m_height;
	m_texture_depth_stencil = std::make_shared<TextureResource2D>();
	m_texture_depth_stencil->InitializeDepth2D(m_device, texture_desc_sized);
}

void RenderAttachment::SetTextureDepthStencil(std::shared_ptr<TextureResource2D> texture)
{
	m_texture_depth_stencil = texture;
}

void RenderAttachment::Bind()
{
	// デバイスコンテキストへレンダーターゲットビューとデプスステンシルを設定
	std::vector<ID3D11RenderTargetView*> rtvs{};
	for (const auto& texture : m_texture_color)
	{
		texture->GetRenderTargetViews(rtvs);
	}
	UINT render_target_cnt = rtvs.size();
	ID3D11RenderTargetView** rtvs_ptr = render_target_cnt > 0 ? rtvs.data() : nullptr;
	ID3D11DepthStencilView* dsv_ptr = m_texture_depth_stencil
		? m_texture_depth_stencil->GetDepthStencilView().Get() : nullptr;
	m_context->OMSetRenderTargets(
		render_target_cnt, rtvs_ptr,
		dsv_ptr);
	// ビューポートの設定
	m_context->RSSetViewports(1, &m_view_port);
}

void RenderAttachment::BindRenderTargetNo(size_t index)
{
	assert(index < m_texture_color.size());
	std::vector<ID3D11RenderTargetView*> rtvs{};
	m_texture_color[index]->GetRenderTargetViews(rtvs);
	UINT render_target_cnt = rtvs.size();
	ID3D11RenderTargetView** rtvs_ptr = render_target_cnt > 0 ? rtvs.data() : nullptr;
	ID3D11DepthStencilView* dsv_ptr = m_texture_depth_stencil
		? m_texture_depth_stencil->GetDepthStencilView().Get() : nullptr;
	m_context->OMSetRenderTargets(
		render_target_cnt, rtvs_ptr,
		dsv_ptr);
	// ビューポートの設定
	m_context->RSSetViewports(1, &m_view_port);
}

void RenderAttachment::ClearColor(const float clear_color[4])
{
	std::vector<ID3D11RenderTargetView*> rtvs{};
	for (const auto& texture : m_texture_color)
	{
		texture->GetRenderTargetViews(rtvs);
	}
	for (const auto& rtv : rtvs)
	{
		m_context->ClearRenderTargetView(rtv, clear_color);
	}
}

void RenderAttachment::ClearDepthStencil(UINT clear_flags, FLOAT depth, UINT8 stencil)
{
	assert(m_texture_depth_stencil);
	m_context->ClearDepthStencilView(m_texture_depth_stencil->GetDepthStencilView().Get(), clear_flags, depth, stencil);
}
