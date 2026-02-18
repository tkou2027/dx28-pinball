#pragma once
#include <memory>
#include <vector>
#include <cassert>
#include <wrl/client.h>
#include "texture_resource.h"
class RenderAttachment
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context,
		int width, int height);
	size_t AddTextureColorFromSwapChain(IDXGISwapChain* swap_chain);
	size_t AddTextureColor(const D3D11_TEXTURE2D_DESC& texture_desc);
	size_t AddTextureColor(std::shared_ptr<TextureResource2D> texture);
	void SetTextureDepthStencil(const D3D11_TEXTURE2D_DESC& texture_desc);
	void SetTextureDepthStencil(std::shared_ptr<TextureResource2D> texture);

	void Resize(int width, int height);

	void Bind();
	void BindRenderTargetNo(size_t index);
	void ClearColor(const float clear_color[4]);
	void ClearDepthStencil(UINT clear_flags, FLOAT depth = 1.0f, UINT8 stencil = 0);

	// getters
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }
	template<class T>
	std::shared_ptr<T> GetTextureColor(size_t index) const
	{
		static_assert(std::is_base_of_v<TextureResourceBase, T>, "Bad texture resource class");
		assert(index < m_texture_color.size());
		return std::dynamic_pointer_cast<T>(m_texture_color[index]);
	}
	std::shared_ptr<TextureResource2D> GetTextureDepthStencil() const { return m_texture_depth_stencil; }
private:
	void SetViewPort();

	uint32_t m_width{};
	uint32_t m_height{};
	// TODO create textures
	D3D11_VIEWPORT m_view_port;
	std::vector<std::shared_ptr<TextureResourceBase>> m_texture_color;
	std::shared_ptr<TextureResource2D> m_texture_depth_stencil;

	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};