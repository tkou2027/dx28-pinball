#pragma once
#include <wrl/client.h>
#include "platform/graphics.h"

class RenderStates
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	void Initialize(ID3D11Device* device);
	void Finalize();

	// predefined dx states
	// rasterizer
	ComPtr<ID3D11RasterizerState> m_rs_cull_back{ nullptr };
	ComPtr<ID3D11RasterizerState> m_rs_cull_front{ nullptr };
	ComPtr<ID3D11RasterizerState> m_rs_cull_none{ nullptr };
	ComPtr<ID3D11RasterizerState> m_rs_depth_bias{ nullptr };
	ComPtr<ID3D11RasterizerState> m_rs_wireframe{ nullptr }; // debug
	// depth stencil
	ComPtr<ID3D11DepthStencilState> m_dss_depth_enabled{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_disabled{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_read{ nullptr }; // skybox
	ComPtr<ID3D11DepthStencilState> m_dss_depth_read_greater{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_read_greater_stencil_read_neq{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_read_stencil_read{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_read_stencil_write{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_enabled_stencil_read{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_read_less_stencil_write_two_pass{ nullptr };
	ComPtr<ID3D11DepthStencilState> m_dss_depth_read_geq{ nullptr };
	// sampler
	ComPtr<ID3D11SamplerState> m_ss_linear_wrap{ nullptr }; // texture
	ComPtr<ID3D11SamplerState> m_ss_point_wrap{ nullptr }; // pixel texture
	ComPtr<ID3D11SamplerState> m_ss_linear_clamp{ nullptr };
	ComPtr<ID3D11SamplerState> m_ss_compare_less_linear_border{ nullptr }; // shadow
	// blend
	ComPtr<ID3D11BlendState> m_bs_disabled{ nullptr };
	ComPtr<ID3D11BlendState> m_bs_add{ nullptr };
	ComPtr<ID3D11BlendState> m_bs_alpha{ nullptr };
	ComPtr<ID3D11BlendState> m_bs_add_alpha{ nullptr };
	float m_blend_factor_zero[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	UINT m_sample_mask_all{ 0xffffffff };
private:
	void InitializeStates();
	ID3D11Device* m_device{ nullptr };
};