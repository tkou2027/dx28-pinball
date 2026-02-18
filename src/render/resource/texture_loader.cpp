#include "texture_loader.h"
#include <memory>
#include "render/dx_trace.h"
#include "render/resource/texture_resource.h"
#include "util/debug_ostream.h"
// direct tex
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/DDSTextureLoader11.h"
// assimp
#include "assimp/texture.h"
// stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex_Release.lib")
#endif // _DEBUG

using namespace DirectX;
using namespace Microsoft::WRL;

void TextureLoader::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swap_chain)
{
	m_device = device;
	m_context = context;
	m_swap_chain = swap_chain;

	// load placeholder textures on application initialize
	LoadPlaceholderTextures();
}

// static utils start ====
ComPtr<ID3D11ShaderResourceView> TextureLoader::LoadTextureFromMemory(ID3D11Device* device, const aiTexture* embedded_texture, bool srgb)
{
	ComPtr<ID3D11ShaderResourceView> texture_srv;
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICMemory(embedded_texture->pcData, embedded_texture->mWidth, WIC_FLAGS_NONE, &metadata, image);

	if (srgb)
	{
		DXGI_FORMAT target_format = MakeSRGB(metadata.format);
		if (target_format == DXGI_FORMAT_UNKNOWN)
		{
			hal::dout << "Warning: MakeSRGB returned DXGI_FORMAT_UNKNOWN for format " << static_cast<int>(metadata.format) << std::endl;
		}
		else if (target_format != metadata.format)
		{
			ScratchImage converted;
			HRESULT hr = Convert(image.GetImages(), image.GetImageCount(), metadata, target_format, TEX_FILTER_DEFAULT, 0.5f, converted);
			if (SUCCEEDED(hr))
			{
				metadata = converted.GetMetadata();
				image = std::move(converted);
			}
			else
			{
				hal::dout << "Warning: Convert to sRGB failed. Falling back to original image." << std::endl;
			}
		}
	}


	HRESULT result = CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), metadata, texture_srv.GetAddressOf());
	if (FAILED(result))
	{
		hal::dout << "Error loading embedded texture " << std::endl;
		return nullptr;
	}
	return texture_srv;
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::LoadTextureFromFile(ID3D11Device* device, const std::string& filename, DXGI_FORMAT format)
{
	return LoadTextureFromFileStb(device, filename, format);
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::LoadTextureFromFileStb(ID3D11Device* device, const std::string& filename, DXGI_FORMAT format)
{
	// load image
	int width, height, channels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4); // 4 channels

	if (!data) {
		hal::dout << "Error loading texture " << filename << std::endl;
		stbi_image_free(data);
		assert(false);
		return nullptr;
	}
	// create texture2D
	ComPtr<ID3D11Texture2D> texture;
	{
		D3D11_TEXTURE2D_DESC td{};
		td.Width = width;
		td.Height = height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = format;
		td.SampleDesc.Count = 1;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data;
		initData.SysMemPitch = width * 4;
		HRESULT hr = device->CreateTexture2D(&td, &initData, &texture);
		if (FAILED(hr)) {
			hal::dout << "Error creating texture " << filename << std::endl;
			stbi_image_free(data);
			assert(false);
			return nullptr;
		}
		stbi_image_free(data);
	}
	// create srv
	ComPtr<ID3D11ShaderResourceView> texture_srv;
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC vd{};
		vd.Format = format;
		vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		vd.Texture2D.MipLevels = 1;
		HRESULT result = device->CreateShaderResourceView(texture.Get(), &vd, texture_srv.GetAddressOf());
		if (FAILED(result))
		{
			hal::dout << "Error creating texture view " << filename << std::endl;
			assert(false);
			return nullptr;
		}
	}
	return texture_srv;
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::LoadTextureCubeFromFile(
	ID3D11Device* device, ID3D11DeviceContext* context,
	const std::string& key, const std::array<std::wstring, 6>& faces)
{
	TexMetadata metadata;
	std::array<ScratchImage, 6>images;

	for (int i = 0; i < 6; ++i)
	{
		HR(LoadFromWICFile(faces[i].c_str(), WIC_FLAGS_NONE, &metadata, images[i]));
		// TODO check size and format

	}
	auto texture_desc = TextureResourceCube::DESC_PREST_CUBE;
	texture_desc.Width = metadata.width;
	texture_desc.Height = metadata.height;
	texture_desc.Format = MakeSRGB(metadata.format); // default to sRGB (for skybox)
	// DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ?
	auto texture_cube = std::make_unique<TextureResourceCube>();
	texture_cube->InitializeTextureCubeNoFaces(device, texture_desc);
	for (uint32_t i = 0; i < 6; ++i)
	{
		const Image* img = images[i].GetImage(0, 0, 0);
		context->UpdateSubresource(texture_cube->GetTexture().Get(),
			D3D11CalcSubresource(0, i, 1), nullptr, img->pixels, img->rowPitch, img->slicePitch);
	}
	return texture_cube->GetCubeShaderResourceView();
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::LoadTextureFromFileDDS(ID3D11Device* device, const std::string& filename)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cube_srv{};
	std::wstring filename_w(filename.begin(), filename.end());
	HR(CreateDDSTextureFromFile(device, filename_w.c_str(), nullptr, cube_srv.GetAddressOf()));
	return cube_srv;
}

// static utils end ====

ComPtr<ID3D11ShaderResourceView> TextureLoader::GetTexture(TextureResourceId resource_id) const
{
	ComPtr<ID3D11ShaderResourceView> srv;
	switch (resource_id.type)
	{
	case TextureType::STATIC:
	{
		srv = GetTextureStatic(resource_id);
		break;
	}
	case TextureType::CAMERA:
	{
		srv = GetTextureCamera(resource_id);
		break;
	}
	}
	// fallback
	if (srv)
	{
		return srv;
	}

	// placeholder
	const int texture_id = m_placeholder_texture_ids[static_cast<size_t>(resource_id.placeholder)];
	assert(texture_id >= 0 && texture_id < m_textures.size());
	return m_textures[texture_id];
}

ComPtr<ID3D11RenderTargetView> TextureLoader::GetRenderTextureRenderTargetView(TextureResourceId resource_id) const
{
	assert(resource_id.type == TextureType::CAMERA);
	const auto& entry = m_render_textures.Get(resource_id.id);
	const auto& texture = entry.textures[entry.rtv_index];
	// must be texture 2d
	const auto& tex_2d = std::get<TextureResource2D>(texture);
	return tex_2d.GetRenderTargetView();
}

ComPtr<ID3D11RenderTargetView> TextureLoader::GetRenderTextureRenderTargetView(TextureResourceId resource_id, int face_id) const
{
	assert(resource_id.type == TextureType::CAMERA);
	const auto& entry = m_render_textures.Get(resource_id.id);
	const auto& texture = entry.textures[entry.rtv_index];
	// must be cube
	const auto& tex_cube = std::get<TextureResourceCube>(texture);
	return tex_cube.GetFaceRenderTargetView(face_id);
}

ComPtr<ID3D11DepthStencilView> TextureLoader::GetRenderTextureDepthStencilView(TextureResourceId resource_id) const
{
	assert(resource_id.type == TextureType::CAMERA);
	const auto& entry = m_render_textures.Get(resource_id.id);
	const auto& texture = entry.textures[entry.rtv_index];
	// must be texture 2d
	const auto& tex_2d = std::get<TextureResource2D>(texture);
	return tex_2d.GetDepthStencilView();
}

TextureResourceId TextureLoader::GetOrLoadTextureFromFile(const std::string& filename, DXGI_FORMAT format)
{
	TextureResourceId resource_id{};
	resource_id.type = TextureType::STATIC;
	// try get
	const int texture_id = GetTextureIdStatic(filename);
	if (texture_id >= 0)
	{
		resource_id.id = texture_id;
		return resource_id;
	}
	// load
	auto texture_srv = LoadTextureFromFile(m_device, filename, format);
	resource_id.id = AddTextureStatic(filename, texture_srv);
	return resource_id;
}

TextureResourceId TextureLoader::GetOrLoadTextureFromFileDDS(const std::string& filename)
{
	TextureResourceId resource_id{};
	resource_id.type = TextureType::STATIC;
	// try get
	const int texture_id = GetTextureIdStatic(filename);
	if (texture_id >= 0)
	{
		resource_id.id = texture_id;
		return resource_id;
	}
	// load
	auto texture_srv = LoadTextureFromFileDDS(m_device, filename);
	resource_id.id = AddTextureStatic(filename, texture_srv);
	return resource_id;
}

TextureResourceId TextureLoader::GetOrLoadTextureCubeFromFile(const std::string& key, const std::array<std::wstring, 6>& faces)
{
	TextureResourceId resource_id{};
	resource_id.type = TextureType::STATIC;
	// try get
	const int texture_id = GetTextureIdStatic(key);
	if (texture_id >= 0)
	{
		resource_id.id = texture_id;
		return resource_id;
	}
	// load
	auto texture_srv = LoadTextureCubeFromFile(m_device, m_context, key, faces);

	resource_id.id = AddTextureStatic(key, texture_srv);
	return resource_id;
}

TextureResourceId TextureLoader::GetOrLoadTextureCubeFromFileDDS(const std::string& filename)
{
	TextureResourceId resource_id{};
	resource_id.type = TextureType::STATIC;
	// try get
	const int texture_id = GetTextureIdStatic(filename);
	if (texture_id >= 0)
	{
		resource_id.id = texture_id;
		return resource_id;
	}
	// load
	auto texture_srv = LoadTextureFromFileDDS(m_device, filename);

	resource_id.id = AddTextureStatic(filename, texture_srv);
	return resource_id;
}

TextureResourceId TextureLoader::GetOrCreateRenderTexture2D(const std::string& camera_key, TextureUsageType type, CD3D11_TEXTURE2D_DESC desc)
{
	int pool_id = m_render_textures.GetId(camera_key);
	if (pool_id >= 0)
	{
		const auto& entry = m_render_textures.Get(pool_id);
		if (entry.initialized)
		{
			TextureResourceId resource_id{};
			resource_id.type = TextureType::CAMERA;
			resource_id.id = pool_id;
			return resource_id;
		}
		// if not initialized, create textures below
	}
	else
	{
		RenderTextureEntry entry{};
		pool_id = m_render_textures.AddIfNotExists(camera_key, entry);
	}

	auto& entry = m_render_textures.Get(pool_id);
	entry.num_copies = (type != TextureUsageType::BACK_BUFFER);
	entry.initialized = true;
	for (int i = 0; i <= entry.num_copies; i++)
	{
		TextureResource2D texture{};
		switch (type)
		{
		case TextureUsageType::RENDER_TARGET:
		{
			texture.InitializeRenderTarget2D(m_device, desc);
			break;
		}
		case TextureUsageType::DEPTH_STENCIL:
		{
			texture.InitializeDepth2D(m_device, desc);
			break;
		}
		case TextureUsageType::BACK_BUFFER:
		{
			texture.InitializeRenderTarget2DFromSwapChain(m_device, m_swap_chain);
			break;
		}
		}
		entry.textures[i] = texture;
	}

	pool_id = m_render_textures.AddIfNotExists(camera_key, entry);
	TextureResourceId resource_id{};
	resource_id.type = TextureType::CAMERA;
	resource_id.id = pool_id;
	return resource_id;
}

TextureResourceId TextureLoader::GetOrCreateRenderTextureCube(const std::string& camera_key, TextureUsageType type, CD3D11_TEXTURE2D_DESC desc)
{
	int pool_id = m_render_textures.GetId(camera_key);
	if (pool_id >= 0)
	{
		const auto& entry = m_render_textures.Get(pool_id);
		if (entry.initialized)
		{
			TextureResourceId resource_id{};
			resource_id.type = TextureType::CAMERA;
			resource_id.id = pool_id;
			return resource_id;
		}
		// if not initialized, create textures below
	}
	else
	{
		RenderTextureEntry entry{};
		pool_id = m_render_textures.AddIfNotExists(camera_key, entry);
	}

	auto& entry = m_render_textures.Get(pool_id);
	entry.num_copies = (type != TextureUsageType::BACK_BUFFER);
	entry.initialized = true;
	for (int i = 0; i <= entry.num_copies; i++)
	{
		TextureResourceCube texture{};
		switch (type)
		{
		case TextureUsageType::RENDER_TARGET:
		{
			texture.InitializeTextureCubeRenderTarget(m_device, desc);
			break;
		}
		default:
		{
			assert(false && "Unsupported texture usage for cube");
			break;
		}
		}
		entry.textures[i] = texture;
	}

	pool_id = m_render_textures.AddIfNotExists(camera_key, entry);
	TextureResourceId resource_id{};
	resource_id.type = TextureType::CAMERA;
	resource_id.id = pool_id;
	return resource_id;
}

TextureResourceId TextureLoader::GetOrCreateRenderTextureId(const std::string& render_texture_key)
{
	int pool_id = m_render_textures.GetId(render_texture_key);
	if (pool_id == -1)
	{
		RenderTextureEntry entry{};
		entry.initialized = false; // not initialized yet
		pool_id = m_render_textures.AddIfNotExists(render_texture_key, entry);
	}

	assert(pool_id >= 0);

	TextureResourceId resource_id{};
	resource_id.type = TextureType::CAMERA;
	resource_id.id = pool_id;
	return resource_id;
}

void TextureLoader::SwapRenderTexture(TextureResourceId resource_id)
{
	int pool_id = resource_id.id;
	assert(pool_id >= 0);
	auto& entry = m_render_textures.Get(resource_id.id);
	assert(entry.initialized);
	if (!entry.num_copies)
	{
		// no srv
		return;
	}
	// swap texture used for srv and rtv
	entry.rtv_index = 1 - entry.rtv_index;
}

void TextureLoader::ResizeRenderTexture(TextureResourceId resource_id, uint32_t width, uint32_t height)
{
	assert(resource_id.type == TextureType::CAMERA);
	auto& entry = m_render_textures.Get(resource_id.id);
	for (int i = 0; i < entry.num_copies; i++)
	{
		auto& texture_variant = entry.textures[i];
		std::visit([&](auto texture) {
			texture.Resize(m_device, width, height);
			}, texture_variant);
	}
}

void TextureLoader::ReleaseRenderTexture(TextureResourceId resource_id)
{
	assert(resource_id.type == TextureType::CAMERA);
	m_render_textures.Remove(resource_id.id);
}

// cache utils ====
int TextureLoader::GetTextureIdStatic(const std::string& key)
{
	auto find_it = m_texture_ids.find(key);
	if (find_it != m_texture_ids.end())
	{
		return find_it->second;
	}
	return -1;
}

int TextureLoader::AddTextureStatic(const std::string& key, ComPtr<ID3D11ShaderResourceView> texture)
{
	int id = m_textures.size();
	m_texture_ids.emplace(key, id);
	m_textures.push_back(texture);
	return id;
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::GetTextureStatic(TextureResourceId resource_id) const
{
	assert(resource_id.type == TextureType::STATIC);
	int texture_id = resource_id.id;
	if (texture_id == -1)
	{
		return nullptr;
	}
	assert(texture_id >= 0 && texture_id < m_textures.size());
	return m_textures[texture_id];
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::GetTextureCamera(TextureResourceId resource_id) const
{
	assert(resource_id.type == TextureType::CAMERA);
	int texture_id = resource_id.id;
	if (texture_id == -1)
	{
		return nullptr;
	}
	assert(texture_id >= 0 && texture_id < m_textures.size());
	const auto& entry = m_render_textures.Get(texture_id);
	assert(entry.num_copies); // must have srv
	const auto& texture_variant = entry.textures[1 - entry.rtv_index];
	if (std::holds_alternative<TextureResource2D>(texture_variant))
	{
		const auto& texture_2d = std::get<TextureResource2D>(texture_variant);
		return texture_2d.GetShaderResourceView();
	}
	else if (std::holds_alternative<TextureResourceCube>(texture_variant))
	{
		const auto& texture_cube = std::get<TextureResourceCube>(texture_variant);
		return texture_cube.GetCubeShaderResourceView();
	}
	return nullptr;
}
// cache utils end ====

void TextureLoader::LoadPlaceholderTextures()
{
	// TODO: config
	constexpr size_t placeholder_num{ static_cast<size_t>(TexturePlaceholder::MAX) };
	std::string placeholder_files[placeholder_num] = {
		"asset/texture/placeholder/white_srgb.png",
		"asset/texture/placeholder/normal.png",
		"asset/texture/placeholder/white.png",
		"asset/texture/placeholder/white.png"
	};
	for (int i = 0; i < placeholder_num; ++i)
	{
		// color as srgb, others as data
		const auto resource_id = GetOrLoadTextureFromFile(placeholder_files[i], i == 0 ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM);
		m_placeholder_texture_ids[i] = resource_id.id;
	}
}