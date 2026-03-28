#include <cassert>
#include "texture_resource.h"
#include "render/util/dx_trace.h"
// utils start
// shadow mapping
static DXGI_FORMAT GetDepthSRVFormat(DXGI_FORMAT texture_format)
{
	switch (texture_format)
	{
	case DXGI_FORMAT_R16_TYPELESS: { return DXGI_FORMAT_R16_FLOAT; }
	case DXGI_FORMAT_R24G8_TYPELESS: { return DXGI_FORMAT_R24_UNORM_X8_TYPELESS; }
	case DXGI_FORMAT_R32_TYPELESS: { return DXGI_FORMAT_R32_FLOAT; }
	case DXGI_FORMAT_R32G8X24_TYPELESS: { return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS; }
	default: { return DXGI_FORMAT_UNKNOWN; }
	}
}

static DXGI_FORMAT GetDepthDSVFormat(DXGI_FORMAT texture_format)
{
	switch (texture_format)
	{
	case DXGI_FORMAT_R16_TYPELESS: { return DXGI_FORMAT_D16_UNORM; }
	case DXGI_FORMAT_R24G8_TYPELESS: { return DXGI_FORMAT_D24_UNORM_S8_UINT; }
	case DXGI_FORMAT_R32_TYPELESS: { return DXGI_FORMAT_D32_FLOAT; }
	case DXGI_FORMAT_R32G8X24_TYPELESS: { return DXGI_FORMAT_D32_FLOAT_S8X24_UINT; }
	default: { return DXGI_FORMAT_UNKNOWN; }
	}
}
// utils end

// static variables

CD3D11_TEXTURE2D_DESC TextureResource::BuildTexturesDesc(
	CD3D11_TEXTURE2D_DESC desc, uint32_t width, uint32_t height)
{
	CD3D11_TEXTURE2D_DESC desc_out = desc;
	desc_out.Width = width;
	desc_out.Height = height;
	return desc_out;
}

const CD3D11_TEXTURE2D_DESC TextureResource2D::DESC_PREST_RENDER_TARGET
{
	DXGI_FORMAT_R8G8B8A8_UNORM, // format
	0, 0,// with, height (placeholder)
	1, // array size
	1, // mip levels (no mip)
	D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET // bindFlags
	// the reset are defaults
};

const CD3D11_TEXTURE2D_DESC TextureResource2D::DESC_PREST_RENDER_TARGET_HDR
{
	DXGI_FORMAT_R16G16B16A16_FLOAT, // format
	0, 0,// with, height (placeholder)
	1, // array size
	1, // mip levels (no mip)
	D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET // bindFlags
	// the reset are defaults
};

const CD3D11_TEXTURE2D_DESC TextureResource2D::DESC_PREST_DEPTH_STENCIL
{
	DXGI_FORMAT_R24G8_TYPELESS, // format
	0, 0,// with, height (placeholder)
	1, // array size
	1, // mip levels (no mip)
	D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL // bindFlags
	// the reset are defaults
};

const CD3D11_TEXTURE2D_DESC TextureResourceCube::DESC_PREST_CUBE
{
	DXGI_FORMAT_R8G8B8A8_UNORM, // format
	0, 0,// with, height (placeholder)
	6, // array size must be 6
	1, // mip levels (no mip)
	D3D11_BIND_SHADER_RESOURCE, // bindFlags
	D3D11_USAGE_DEFAULT, // usage
	0, 1, 0, // CPUAccessFlags, sampleCount, sampleQuality
	D3D11_RESOURCE_MISC_TEXTURECUBE // must be cube
};

const CD3D11_TEXTURE2D_DESC TextureResourceCube::DESC_PREST_CUBE_TARGET
{
	DXGI_FORMAT_R8G8B8A8_UNORM, // format
	0, 0,// with, height (placeholder)
	6, // array size must be 6
	1, // mip levels (no mip)
	D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, // bindFlags
	D3D11_USAGE_DEFAULT, // usage
	0, 1, 0, // CPUAccessFlags, sampleCount, sampleQuality
	D3D11_RESOURCE_MISC_TEXTURECUBE // must be cube
};


CD3D11_TEXTURE2D_DESC TextureResource2D::GetTextureDesc(DXGI_FORMAT format, UINT bind)
{
	return CD3D11_TEXTURE2D_DESC{
		format, // format
		0, 0,// with, height (placeholder)
		1, // array size
		1, // mip levels (no mip)
		bind // bindFlags
		// the reset are defaults
	};
}

// static variables end

// texture 2d start ========
void TextureResource2D::CreateTexture2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	m_texture.Reset();
	m_width = texture_desc.Width;
	m_height = texture_desc.Height;
	HR(device->CreateTexture2D(&texture_desc, nullptr, m_texture.GetAddressOf()));
}

void TextureResource2D::CreateShaderResourceView(ID3D11Device* device, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvd)
{
	m_srv.Reset();
	HR(device->CreateShaderResourceView(m_texture.Get(), &srvd, m_srv.GetAddressOf()));
}

void TextureResource2D::CreateDepthStencilView(ID3D11Device* device, const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvd)
{
	m_dsv.Reset();
	HR(device->CreateDepthStencilView(m_texture.Get(), &dsvd, m_dsv.GetAddressOf()));
}

void TextureResource2D::CreateDepthStencilViewReadOnly(ID3D11Device* device, const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvd)
{
	m_dsv_read_only.Reset();
	// TODO validate
	HR(device->CreateDepthStencilView(m_texture.Get(), &dsvd, m_dsv_read_only.GetAddressOf()));
}

void TextureResource2D::CreateRenderTargetView(ID3D11Device* device)
{
	m_rtv.Reset();
	HR(device->CreateRenderTargetView(m_texture.Get(), nullptr, m_rtv.GetAddressOf()));
}

void TextureResource2D::CreateUnorderedAccessView(ID3D11Device* device, const D3D11_UNORDERED_ACCESS_VIEW_DESC& uavd)
{
	m_uav.Reset();
	HR(device->CreateUnorderedAccessView(m_texture.Get(), &uavd, m_uav.GetAddressOf()));
}

void TextureResource2D::Resize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;

	// create from swap chain
	if (m_from_swap_chain)
	{
		InitializeRenderTarget2DFromSwapChain(device, m_swap_chain);
		return;
	}

	if (m_texture)
	{
		D3D11_TEXTURE2D_DESC texture_desc{};
		m_texture->GetDesc(&texture_desc);
		texture_desc.Width = m_width;
		texture_desc.Height = m_height;
		CreateTexture2D(device, texture_desc);
	}
	if (m_srv)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		m_srv->GetDesc(&srvd);
		CreateShaderResourceView(device, srvd);
	}
	if (m_rtv)
	{
		// TODO
		//D3D11_RENDER_TARGET_VIEW_DESC rtvd{};
		//m_rtv->GetDesc(&rtvd);
		CreateRenderTargetView(device);
	}
	if (m_dsv)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd{};
		m_dsv->GetDesc(&dsvd);
		CreateDepthStencilView(device, dsvd);
	}
	if (m_dsv_read_only)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd{};
		m_dsv_read_only->GetDesc(&dsvd);
		CreateDepthStencilViewReadOnly(device, dsvd);
	}
	if (m_uav)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
		m_uav->GetDesc(&uavd);
		CreateUnorderedAccessView(device, uavd);
	}
}

void TextureResource2D::GetRenderTargetViews(std::vector<ID3D11RenderTargetView*>& rtvs) const
{
	auto rtv = m_rtv.Get();
	if (rtv)
	{
		rtvs.push_back(rtv);
	}
}

void TextureResource2D::InitializeRenderTarget2DFromSwapChain(ID3D11Device* device, IDXGISwapChain* swap_chain)
{
	ResetAll();

	m_from_swap_chain = true;
	m_swap_chain = swap_chain;
	// レンダーターゲットビュー生成
	ComPtr<ID3D11Texture2D> back_buffer;
	m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)back_buffer.GetAddressOf());
	device->CreateRenderTargetView(back_buffer.Get(), nullptr, m_rtv.GetAddressOf());
}

void TextureResource2D::InitializeRenderTarget2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	const auto bind_flags{ texture_desc.BindFlags };
	assert(bind_flags & D3D11_BIND_RENDER_TARGET, "Bad binding flag for render target texture"); // check binding flags
	// TODO validate format
	const auto format{ texture_desc.Format };

	ResetAll();
	CreateTexture2D(device, texture_desc);
	if (bind_flags & D3D11_BIND_SHADER_RESOURCE)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(D3D11_SRV_DIMENSION_TEXTURE2D, format);
		CreateShaderResourceView(device, srvd);
	}
	CreateRenderTargetView(device);

	// create UAV if requested
	//if (bind_flags & D3D11_BIND_UNORDERED_ACCESS)
	//{
	//	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
	//	uavd.Format = format;
	//	uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	//	uavd.Texture2D.MipSlice = 0;
	//	CreateUnorderedAccessView(device, uavd);
	//}
}

void TextureResource2D::InitializeDepth2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	// TODO: check binding flag
	const auto bind_flags{ texture_desc.BindFlags };
	assert(bind_flags & D3D11_BIND_DEPTH_STENCIL, "Bad binding flag for depth stencil texture"); // check binding flags

	const auto format{ texture_desc.Format };
	const auto format_dsv{ GetDepthDSVFormat(format) };
	assert(format_dsv != DXGI_FORMAT_UNKNOWN, "Bad format for depth stencil texture"); // check format

	ResetAll();
	CreateTexture2D(device, texture_desc);
	if (bind_flags & D3D11_BIND_SHADER_RESOURCE)
	{
		const auto format_srv{ GetDepthSRVFormat(format) };
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(D3D11_SRV_DIMENSION_TEXTURE2D, format_srv);
		CreateShaderResourceView(device, srvd);
	}
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D, format_dsv);
	CreateDepthStencilView(device, dsvd);

	//// create UAV for depth if requested (use SRV-compatible format)
	//if (bind_flags & D3D11_BIND_UNORDERED_ACCESS)
	//{
	//	const auto format_srv{ GetDepthSRVFormat(format) };
	//	if (format_srv != DXGI_FORMAT_UNKNOWN)
	//	{
	//		D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
	//		uavd.Format = format_srv;
	//		uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	//		uavd.Texture2D.MipSlice = 0;
	//		CreateUnorderedAccessView(device, uavd);
	//	}
	//}
}

void TextureResource2D::InitializeDepth2DWithReadOnly(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	InitializeDepth2D(device, texture_desc);
	const auto format{ texture_desc.Format };
	const auto format_dsv{ GetDepthDSVFormat(format) };
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd{ D3D11_DSV_DIMENSION_TEXTURE2D, format_dsv };
	dsvd.Flags |= D3D11_DSV_READ_ONLY_DEPTH;
	dsvd.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
	CreateDepthStencilViewReadOnly(device, dsvd);
}

void TextureResource2D::InitializeUnorderedAccess2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	const auto bind_flags{ texture_desc.BindFlags };
	assert(bind_flags & D3D11_BIND_UNORDERED_ACCESS); // check binding flags
	// TODO validate format
	const auto format{ texture_desc.Format };

	ResetAll();
	CreateTexture2D(device, texture_desc);
	if (bind_flags & D3D11_BIND_SHADER_RESOURCE)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(D3D11_SRV_DIMENSION_TEXTURE2D, format);
		CreateShaderResourceView(device, srvd);
	}
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
	uavd.Format = format;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavd.Texture2D.MipSlice = 0;
	CreateUnorderedAccessView(device, uavd);
}

void TextureResource2D::ResetAll()
{
	m_texture.Reset();
	m_srv.Reset();
	m_rtv.Reset();
	m_uav.Reset();
	m_dsv.Reset();
	m_dsv_read_only.Reset();
	m_from_swap_chain = false;
	m_swap_chain = nullptr;
}

// texture 2d end ========


// texture cube start ========
// DirectX API
void TextureResourceCube::CreateTexture2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	// texture
	const auto misc_flags{ texture_desc.MiscFlags };
	assert(misc_flags & D3D11_RESOURCE_MISC_TEXTURECUBE);
	m_width = texture_desc.Width;
	m_height = texture_desc.Height;
	HR(device->CreateTexture2D(&texture_desc, nullptr, m_texture.ReleaseAndGetAddressOf()));
}

void TextureResourceCube::CreateCubeShaderResourceView(ID3D11Device* device, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvd)
{
	assert(m_texture.Get());
	HR(device->CreateShaderResourceView(m_texture.Get(), &srvd, m_srv_cube.ReleaseAndGetAddressOf()));
}

void TextureResourceCube::CreateFaceRenderTargetViews(ID3D11Device* device, const CD3D11_RENDER_TARGET_VIEW_DESC& rtvd)
{
	assert(m_texture.Get());
	const auto view_dimension = rtvd.ViewDimension;
	assert(view_dimension == D3D11_RTV_DIMENSION_TEXTURE2DARRAY);
	CD3D11_RENDER_TARGET_VIEW_DESC rtvd_face{ rtvd };
	rtvd_face.Texture2DArray.ArraySize = 1;
	for (int i = 0; i < FACE_NUM; i++)
	{
		rtvd_face.Texture2DArray.FirstArraySlice = i;
		HR(device->CreateRenderTargetView(m_texture.Get(), &rtvd_face, m_rtv_faces[i].ReleaseAndGetAddressOf()));
	}
}

void TextureResourceCube::Resize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;

	if (m_texture)
	{
		D3D11_TEXTURE2D_DESC texture_desc{};
		m_texture->GetDesc(&texture_desc);
		texture_desc.Width = m_width;
		texture_desc.Height = m_height;
		CreateTexture2D(device, texture_desc);
	}
	if (m_srv_cube)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		m_srv_cube->GetDesc(&srvd);
		CreateCubeShaderResourceView(device, srvd);
	}
	if (m_rtv_faces[0])
	{
		CD3D11_RENDER_TARGET_VIEW_DESC rtvd{};
		m_rtv_faces[0]->GetDesc(&rtvd);
		CreateFaceRenderTargetViews(device, rtvd);
	}
}

void TextureResourceCube::InitializeTextureCubeNoFaces(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	ResetAll();

	// texture
	CreateTexture2D(device, texture_desc);

	const auto format{ texture_desc.Format }; // TODO: check format
	const auto bind_flags{ texture_desc.BindFlags };
	// cube srv
	if (bind_flags & D3D11_BIND_SHADER_RESOURCE)
	{
		// a srv for texture cube
		const auto cube_srvd = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURECUBE, format);
		CreateCubeShaderResourceView(device, cube_srvd);
	}
}

void TextureResourceCube::InitializeTextureCubeRenderTarget(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	ResetAll();

	// texture
	CreateTexture2D(device, texture_desc);

	const auto format{ texture_desc.Format }; // TODO: check format
	const auto bind_flags{ texture_desc.BindFlags };
	// cube srv
	if (bind_flags & D3D11_BIND_SHADER_RESOURCE)
	{
		// a srv for texture cube
		const auto cube_srvd = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURECUBE, format);
		CreateCubeShaderResourceView(device, cube_srvd);
	}

	// rtv
	if (bind_flags & D3D11_BIND_RENDER_TARGET)
	{
		CD3D11_RENDER_TARGET_VIEW_DESC rtv_desc(
			D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
			format,
			0, // MipSlice
			0, // FirstArraySlice(placeholder)
			1 // ArraySize
		);
		CreateFaceRenderTargetViews(device, rtv_desc);
	}
}

void TextureResourceCube::ResetAll()
{
	m_texture.Reset();
	m_srv_cube.Reset();
	for (int i = 0; i < FACE_NUM; i++)
	{
		m_rtv_faces[i].Reset();
	}
}
// texture cube end ========

void TextureResource2DReadWrite::Initialize(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc)
{
	const auto format{ texture_desc.Format }; // TODO: check format
	const auto bind_flags{ texture_desc.BindFlags };
	assert(bind_flags & D3D11_BIND_SHADER_RESOURCE);
	assert(bind_flags & D3D11_BIND_UNORDERED_ACCESS);

	CreateTexture2D(device, texture_desc);
	// srv for all mip levels
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(D3D11_SRV_DIMENSION_TEXTURE2D, format);
	CreateShaderResourceView(device, srvd);
	// srv and uav for each mip level
	CreateViewsPerLevel(device);
}

// texture 2d read write start ========
void TextureResource2DReadWrite::Resize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	TextureResource2D::Resize(device, width, height);
	CreateViewsPerLevel(device);
}

void TextureResource2DReadWrite::ResetAll()
{
	TextureResource2D::ResetAll();
	m_srvs_per_level.clear();
	m_uavs_per_level.clear();
}

void TextureResource2DReadWrite::CreateViewsPerLevel(ID3D11Device* device)
{
	m_srvs_per_level.clear();
	m_uavs_per_level.clear();
	// get actual mip levels
	D3D11_TEXTURE2D_DESC actual_desc{};
	this->GetTexture()->GetDesc(&actual_desc);
	const UINT actual_mips = actual_desc.MipLevels;

	// create views for each mip level
	m_srvs_per_level.assign(actual_mips, nullptr);
	m_uavs_per_level.assign(actual_mips, nullptr);
	for (UINT i = 0; i < actual_mips; ++i)
	{
		// srv
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = actual_desc.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = i;
		srvd.Texture2D.MipLevels = 1;
		HR(device->CreateShaderResourceView(this->GetTexture().Get(), &srvd, &m_srvs_per_level[i]));

		// uav
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
		uavd.Format = actual_desc.Format;
		uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavd.Texture2D.MipSlice = i;
		HR(device->CreateUnorderedAccessView(this->GetTexture().Get(), &uavd, &m_uavs_per_level[i]));
	}
}
