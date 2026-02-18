#pragma once
#include <string>
#include <array>
#include <variant>
#include <unordered_map>
#include "platform/graphics.h"
#include "render/config/texture_resource_id.h"
#include "render/resource/texture_resource.h"
#include "util/resource_pool.h"

struct aiTexture;
class TextureLoader
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* m_swap_chain);
	// static utils
	static ComPtr<ID3D11ShaderResourceView> LoadTextureFromMemory(
		ID3D11Device* device, const aiTexture* embedded_texture, bool srgb = false);
	static ComPtr<ID3D11ShaderResourceView> LoadTextureFromFile(
		ID3D11Device* device, const std::string& filename, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	// get
	ComPtr<ID3D11ShaderResourceView> GetTexture(TextureResourceId resource_id) const;
	// get camera views
	ComPtr<ID3D11RenderTargetView> GetRenderTextureRenderTargetView(TextureResourceId resource_id) const;
	ComPtr<ID3D11RenderTargetView> GetRenderTextureRenderTargetView(TextureResourceId resource_id, int face_id) const; // for cube
	ComPtr<ID3D11DepthStencilView> GetRenderTextureDepthStencilView(TextureResourceId resource_id) const;
	// create from file
	TextureResourceId GetOrLoadTextureFromFile(const std::string& filename, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	TextureResourceId GetOrLoadTextureFromFileDDS(const std::string& filename);
	TextureResourceId GetOrLoadTextureCubeFromFile(const std::string& key, const std::array<std::wstring, 6>& faces);
	TextureResourceId GetOrLoadTextureCubeFromFileDDS(const std::string& filename);
	// create for camera
	TextureResourceId GetOrCreateRenderTexture2D(const std::string& camera_key, TextureUsageType type, CD3D11_TEXTURE2D_DESC desc);
	TextureResourceId GetOrCreateRenderTextureCube(const std::string& camera_key, TextureUsageType type, CD3D11_TEXTURE2D_DESC desc);
	TextureResourceId GetOrCreateRenderTextureId(const std::string& render_texture_key); // create empty id even if texture not initialized
	void SwapRenderTexture(TextureResourceId resource_id);
	void ResizeRenderTexture(TextureResourceId resource_id, uint32_t width, uint32_t height);
	void ReleaseRenderTexture(TextureResourceId resource_id);
private:
	// static utils
	static ComPtr<ID3D11ShaderResourceView> LoadTextureFromFileStb(ID3D11Device* device, const std::string& filename, DXGI_FORMAT format);
	static ComPtr<ID3D11ShaderResourceView> LoadTextureCubeFromFile(
		ID3D11Device* device, ID3D11DeviceContext* m_context, const std::string& key, const std::array<std::wstring, 6>& faces);
	static ComPtr<ID3D11ShaderResourceView> LoadTextureFromFileDDS(ID3D11Device* device, const std::string& filename);
	// cache utils
	int GetTextureIdStatic(const std::string& key);
	int AddTextureStatic(const std::string& key, ComPtr<ID3D11ShaderResourceView> texture);
	// get texture srv utils
	ComPtr<ID3D11ShaderResourceView> GetTextureStatic(TextureResourceId resource_id) const;
	ComPtr<ID3D11ShaderResourceView> GetTextureCamera(TextureResourceId resource_id) const;
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
	IDXGISwapChain* m_swap_chain{ nullptr };

	void LoadPlaceholderTextures();

	// static textures
	std::unordered_map<std::string, int> m_texture_ids;
	std::vector<ComPtr<ID3D11ShaderResourceView>> m_textures;
	std::array<int, static_cast<size_t>(TexturePlaceholder::MAX)> m_placeholder_texture_ids;

	// render textures
	struct RenderTextureEntry
	{
		// keep a copy of texture to avoid binding srv and rtv at to same time
		std::variant<TextureResource2D, TextureResourceCube> textures[2];
		uint16_t rtv_index{ 0 };
		bool num_copies{ true }; // false for back buffer, no srv
		bool initialized{ false }; // allow using id in texture befor camera initialization
	};
	ResourcePool<RenderTextureEntry> m_render_textures{};
};