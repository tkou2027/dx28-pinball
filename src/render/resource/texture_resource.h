#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include "render/directx.h"

namespace TextureResource
{
	CD3D11_TEXTURE2D_DESC BuildTexturesDesc(CD3D11_TEXTURE2D_DESC desc, uint32_t width, uint32_t height);
	//const static CD3D11_TEXTURE2D_DESC DESC_PREST_RENDER_TARGET;
	//const static CD3D11_TEXTURE2D_DESC DESC_PREST_RENDER_TARGET_HDR;
	//const static CD3D11_TEXTURE2D_DESC DESC_PREST_DEPTH_STENCIL;
	//const static CD3D11_TEXTURE2D_DESC DESC_PREST_CUBE;
}

class TextureResourceBase
{
public:
	virtual ~TextureResourceBase() = default;
	// getters
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }
	virtual void Resize(ID3D11Device* device, uint32_t width, uint32_t height) = 0;
	virtual Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const = 0;
	virtual void GetRenderTargetViews(std::vector<ID3D11RenderTargetView*>& rtvs) const = 0;
	
protected:
	// texture size
	uint32_t m_width{};
	uint32_t m_height{};
};

class TextureResource2D : public TextureResourceBase
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// raw DirectX api with some validations
	void CreateTexture2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc);
	void CreateShaderResourceView(ID3D11Device* device, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvd);
	void CreateDepthStencilView(ID3D11Device* device, const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvd);
	void CreateRenderTargetView(ID3D11Device* device);

	// getters
	ComPtr<ID3D11Texture2D> GetTexture() const override { return m_texture; }
	ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const { return m_srv; }
	ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const { return m_rtv; }
	ComPtr<ID3D11DepthStencilView> GetDepthStencilView() const { return m_dsv; }

	// common interface
	void Resize(ID3D11Device* device, uint32_t width, uint32_t height) override;
	void GetRenderTargetViews(std::vector<ID3D11RenderTargetView*>& rtvs) const override;

	// helpers and presets for frequently used settings
	void InitializeRenderTarget2DFromSwapChain(ID3D11Device* device, IDXGISwapChain* swap_chain);
	void InitializeRenderTarget2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc);
	void InitializeDepth2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc);
	// helpers preset
	const static CD3D11_TEXTURE2D_DESC DESC_PREST_RENDER_TARGET;
	const static CD3D11_TEXTURE2D_DESC DESC_PREST_RENDER_TARGET_HDR;
	const static CD3D11_TEXTURE2D_DESC DESC_PREST_DEPTH_STENCIL;
	const static CD3D11_TEXTURE2D_DESC DESC_PREST_RENDER_TARGET_AUTO_MIP;
	static CD3D11_TEXTURE2D_DESC GetTextureDesc(DXGI_FORMAT format, UINT bind);
private:
	void ResetAll();
	// texture2d
	ComPtr<ID3D11Texture2D> m_texture{ nullptr };
	// views
	ComPtr<ID3D11ShaderResourceView> m_srv{ nullptr };
	ComPtr<ID3D11RenderTargetView> m_rtv{ nullptr };
	ComPtr<ID3D11DepthStencilView> m_dsv{ nullptr };
	// back buffer
	bool m_from_swap_chain{ false };
	IDXGISwapChain* m_swap_chain{ nullptr };
};

class TextureResourceCube : public TextureResourceBase
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// raw DirectX api with some validations, create form desc
	void CreateTexture2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc);
	void CreateCubeShaderResourceView(ID3D11Device* device, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvd);
	// void CreateFaceShaderResourceViews(const D3D11_SHADER_RESOURCE_VIEW_DESC& srvd);
	void CreateFaceRenderTargetViews(ID3D11Device* device, const CD3D11_RENDER_TARGET_VIEW_DESC& rtvd);

	// getters
	ComPtr<ID3D11Texture2D> GetTexture() const override { return m_texture; }
	ComPtr<ID3D11ShaderResourceView> GetCubeShaderResourceView() const { return m_srv_cube; }
	// ComPtr<ID3D11ShaderResourceView> GetFaceShaderResourceView(size_t index) const { return m_srv_faces.at(index); }
	ComPtr<ID3D11RenderTargetView> GetFaceRenderTargetView(size_t index) const { return m_rtv_faces.at(index); }

	// common interfaces
	void Resize(ID3D11Device* device, uint32_t width, uint32_t height) override;
	void GetRenderTargetViews(std::vector<ID3D11RenderTargetView*>& rtvs) const override {}; // TODO

	// helpers and presets for frequently used settings
	void InitializeTextureCubeNoFaces(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc);
	void InitializeTextureCubeRenderTarget(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texture_desc);

	const static CD3D11_TEXTURE2D_DESC DESC_PREST_CUBE;
	const static CD3D11_TEXTURE2D_DESC DESC_PREST_CUBE_TARGET;
private:
	void ResetAll();
	ComPtr<ID3D11Texture2D> m_texture{ nullptr };
	// texture cube srv
	ComPtr<ID3D11ShaderResourceView> m_srv_cube{ nullptr }; // texture array
	// resources for individual faces
	static constexpr int FACE_NUM{ 6 };
	// std::array<ComPtr<ID3D11ShaderResourceView>, FACE_NUM> m_srv_faces;
	std::array<ComPtr<ID3D11RenderTargetView>, FACE_NUM> m_rtv_faces;
};
