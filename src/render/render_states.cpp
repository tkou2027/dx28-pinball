#include "render_states.h"
#include "dx_trace.h"

void RenderStates::Initialize(ID3D11Device* device)
{
	m_device = device;
	InitializeStates();
}

void RenderStates::Finalize()
{
	// ComPtr
	// nothing to do here
}

void RenderStates::InitializeStates()
{
	// rasterizer
	{
		// cull back
		D3D11_RASTERIZER_DESC rd = {};
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_BACK;
		rd.DepthClipEnable = TRUE;
		rd.MultisampleEnable = FALSE; // MSAA
		HR(m_device->CreateRasterizerState(&rd, m_rs_cull_back.GetAddressOf()));
	}
	{
		// cull front
		D3D11_RASTERIZER_DESC rd = {};
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_FRONT;
		rd.DepthClipEnable = TRUE;
		rd.MultisampleEnable = FALSE; // MSAA
		HR(m_device->CreateRasterizerState(&rd, m_rs_cull_front.GetAddressOf()));
	}
	{
		// cull none
		D3D11_RASTERIZER_DESC rd = {};
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.DepthClipEnable = TRUE;
		rd.MultisampleEnable = FALSE; // MSAA
		HR(m_device->CreateRasterizerState(&rd, m_rs_cull_none.GetAddressOf()));
	}
	{
		// shadow
		CD3D11_RASTERIZER_DESC rd(CD3D11_DEFAULT{});
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.FrontCounterClockwise = false;
		rd.DepthBias = 200000;
		rd.DepthBiasClamp = 0.0f;
		rd.SlopeScaledDepthBias = 2.0f;
		HR(m_device->CreateRasterizerState(&rd, m_rs_depth_bias.GetAddressOf()));
	}
	{
		// wireframe
		D3D11_RASTERIZER_DESC rd = {};
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		rd.DepthClipEnable = TRUE;
		rd.MultisampleEnable = FALSE; // MSAA
		HR(m_device->CreateRasterizerState(&rd, m_rs_wireframe.GetAddressOf()));
	}
	// depth stencil
	{
		// enable depth read(less equal) / write, 
		D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthEnable = TRUE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		// disable stencil
		dsd.StencilEnable = FALSE;
		m_device->CreateDepthStencilState(&dsd, m_dss_depth_enabled.GetAddressOf());
	}
	{
		// disable depth
		D3D11_DEPTH_STENCIL_DESC dsd{};
		dsd.DepthEnable = FALSE;
		// disable stencil
		dsd.StencilEnable = FALSE;
		HR(m_device->CreateDepthStencilState(&dsd, m_dss_depth_disabled.GetAddressOf()));
	}
	{
		// enable depth read(less equal), no write 
		D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthEnable = TRUE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable write
		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		// disable stencil
		dsd.StencilEnable = FALSE;
		m_device->CreateDepthStencilState(&dsd, m_dss_depth_read.GetAddressOf());
	}
	{
		// enable depth read(less equal), no write 
		D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthEnable = TRUE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable write
		dsd.DepthFunc = D3D11_COMPARISON_GREATER;
		// disable stencil
		dsd.StencilEnable = FALSE;
		m_device->CreateDepthStencilState(&dsd, m_dss_depth_read_greater.GetAddressOf());
	}
	{
		// enable depth read(less equal), no write 
		D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthEnable = TRUE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable write
		dsd.DepthFunc = D3D11_COMPARISON_GREATER;
		// enable stencil read
		dsd.StencilEnable = TRUE;
		dsd.StencilReadMask = 0xFF;
		dsd.StencilWriteMask = 0x00; // no write
		// front face
		dsd.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL; // greater
		dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		// back face
		dsd.BackFace = dsd.FrontFace;
		m_device->CreateDepthStencilState(&dsd, m_dss_depth_read_greater_stencil_read_neq.GetAddressOf());
	}
	{
		// enable depth read(less equal), no write 
		D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthEnable = TRUE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable write
		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		// enable stencil read
		dsd.StencilEnable = TRUE;
		dsd.StencilReadMask = 0xFF;
		dsd.StencilWriteMask = 0x00; // no write
		// front face
		dsd.FrontFace.StencilFunc = D3D11_COMPARISON_LESS; // greater
		dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		// back face
		dsd.BackFace = dsd.FrontFace;
		m_device->CreateDepthStencilState(&dsd, m_dss_depth_read_stencil_read.GetAddressOf());
	}
	{
		D3D11_DEPTH_STENCIL_DESC desc{};
		// enable depth read(less equal), disable depth write
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable write
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		// enable stencil write
		desc.StencilEnable = TRUE;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;
		// front face
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;// write stencil
		// back face
		desc.BackFace = desc.FrontFace;
		HR(m_device->CreateDepthStencilState(&desc, m_dss_depth_read_stencil_write.GetAddressOf()));
	}
	{
		D3D11_DEPTH_STENCIL_DESC desc{};
		// enable depth
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		// enable stencil read
		desc.StencilEnable = TRUE;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0x00; // no write
		// front face
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_LESS; // greater
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		// back face
		desc.BackFace = desc.FrontFace;
		HR(m_device->CreateDepthStencilState(&desc, m_dss_depth_enabled_stencil_read.GetAddressOf()));
	}
	{
		D3D11_DEPTH_STENCIL_DESC desc{};
		// enable depth read
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable write
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		// enable stencil read write
		desc.StencilEnable = TRUE;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;
		// front face
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // always
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR; // front face + 1
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		// back face
		desc.BackFace = desc.FrontFace;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR; // back face - 1
		HR(m_device->CreateDepthStencilState(&desc, m_dss_depth_read_less_stencil_write_two_pass.GetAddressOf()));
	}
	{
		D3D11_DEPTH_STENCIL_DESC desc{};
		// enable depth read
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable write
		desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		// enable stencil read write
		desc.StencilEnable = FALSE;
		HR(m_device->CreateDepthStencilState(&desc, m_dss_depth_read_geq.GetAddressOf()));
	}
	// sampler
	{
		D3D11_SAMPLER_DESC sd{};
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		sd.MipLODBias = 0.0f;
		sd.MaxAnisotropy = 1;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		HR(m_device->CreateSamplerState(&sd, m_ss_linear_wrap.GetAddressOf()));
	}
	{
		D3D11_SAMPLER_DESC sd{};
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias = 0;
		sd.MaxAnisotropy = 16;
		sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		HR(m_device->CreateSamplerState(&sd, m_ss_point_wrap.GetAddressOf()));
	}
	{
		D3D11_SAMPLER_DESC sd{};
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		sd.MipLODBias = 0.0f;
		sd.MaxAnisotropy = 1;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		HR(m_device->CreateSamplerState(&sd, m_ss_linear_clamp.GetAddressOf()));
	}
	{
		D3D11_SAMPLER_DESC sd = {};
		sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.ComparisonFunc = D3D11_COMPARISON_LESS;
		sd.MaxAnisotropy = 1;
		sd.MinLOD = 0.0f;
		sd.MaxLOD = 0.0f;
		sd.BorderColor[0] = 0.0f;
		sd.BorderColor[1] = 0.0f;
		sd.BorderColor[2] = 0.0f;
		sd.BorderColor[3] = 1.0f;
		HR(m_device->CreateSamplerState(&sd, m_ss_compare_less_linear_border.GetAddressOf()));
	}
	// blend states
	{

		// ブレンドステート設定
		D3D11_BLEND_DESC bd = {};
		bd.AlphaToCoverageEnable = FALSE;
		// same setting for all targets, only target0 used
		bd.IndependentBlendEnable = FALSE;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		// disable blend
		bd.RenderTarget[0].BlendEnable = FALSE;
		HR(m_device->CreateBlendState(&bd, m_bs_disabled.GetAddressOf()));

		// enable blend
		bd.RenderTarget[0].BlendEnable = TRUE;
		// alpha blends
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;



		HR(m_device->CreateBlendState(&bd, m_bs_alpha.GetAddressOf()));
		// 加算合成
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		HR(m_device->CreateBlendState(&bd, m_bs_add_alpha.GetAddressOf()));
		// additive lighting
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		HR(m_device->CreateBlendState(&bd, m_bs_add.GetAddressOf()));
	}
}
