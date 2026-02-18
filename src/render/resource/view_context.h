#pragma once
#include <vector>
#include <optional>
#include "render/directx.h"
#include "render/config/model_state.h"
#include "render/resource/texture_resource.h"

class RenderTarget
{
public:
	void Initialize(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;
		SetViewPortDefault();
	}
	void AddRenderTarget(const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv)
	{
		m_render_targets.push_back(rtv);
	}
	void SetDepthStencil(const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& dsv)
	{
		m_depth_stencil = dsv;
	}
	void Reset(uint32_t width, uint32_t height)
	{
		m_render_targets.clear();
		m_depth_stencil.Reset();
		m_width = width;
		m_height = height;
		SetViewPortDefault();
	}
	void GetSize(uint32_t& width, uint32_t& height) const
	{
		width = m_width;
		height = m_height;
	}
	void Bind(ID3D11DeviceContext* context) const
	{
		UINT render_target_cnt = m_render_targets.size();
		// make rtv pointer array
		std::vector<ID3D11RenderTargetView*> rtvs{};
		rtvs.reserve(render_target_cnt);
		for (const auto& rtv : m_render_targets)
		{
			rtvs.push_back(rtv.Get());
		}
		ID3D11RenderTargetView** rtvs_ptr = render_target_cnt > 0 ? rtvs.data() : nullptr;
		// set textures
		context->OMSetRenderTargets(
			render_target_cnt, rtvs_ptr,
			m_depth_stencil.Get()
		);

		// set view port
		context->RSSetViewports(1, &m_view_port);
	}
	void Unbind(ID3D11DeviceContext* context) const
	{
		// set null render targets
		std::vector<ID3D11RenderTargetView*> null_rtvs(m_render_targets.size(), nullptr);
		context->OMSetRenderTargets(
			static_cast<UINT>(null_rtvs.size()), null_rtvs.data(),
			nullptr
		);
	}
	void ClearColor(ID3D11DeviceContext* context, const float clear_color[4]) const
	{
		for (const auto& rtv : m_render_targets)
		{
			context->ClearRenderTargetView(rtv.Get(), clear_color);
		}
	}
	void ClearDepthStencil(ID3D11DeviceContext* context, UINT clear_flags, FLOAT depth = 1.0f, UINT8 stencil = 0) const
	{
		if (!m_depth_stencil)
		{
			return;
		}
		context->ClearDepthStencilView(m_depth_stencil.Get(), clear_flags, depth, stencil);
	}
	void AdjustViewPort(uint32_t window_width, uint32_t window_height);
private:
	void SetViewPortDefault()
	{
		m_view_port.Width = static_cast<float>(m_width);
		m_view_port.Height = static_cast<float>(m_height);
		m_view_port.MinDepth = 0.0f;
		m_view_port.MaxDepth = 1.0f;
		m_view_port.TopLeftX = 0.0f;
		m_view_port.TopLeftY = 0.0f;
	}

	uint32_t m_width{};
	uint32_t m_height{};
	D3D11_VIEWPORT m_view_port{};
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView> > m_render_targets;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stencil;
};

struct ViewObjects
{
	std::vector<ModelRenderInfo* > objects_static;
	std::vector<ModelRenderInfo* > objects_skinned;
};

struct ViewContext
{
	// camera constant buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_per_projection;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_per_view;

	// RenderTarget
	RenderTarget render_target_out;

	// visible(TODO: not implemented) objects
	ViewObjects view_objects;
};
