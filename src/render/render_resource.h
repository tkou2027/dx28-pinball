#pragma once
#include <variant>
#include "render/directx.h"
#include "render/resource/texture_loader.h"
#include "render/resource/model_loader.h"
#include "render/resource/view_context.h"
#include "render/resource/texture_resource.h"
#include "render/config/camera_data.h"

#include "render/config/material_desc.h"
#include "util/resource_pool.h"

class MaterialResourceManager
{
public:
	void Initialize(); // load default materials
	int AddMaterialDescWithGeneratedKey(const MaterialDesc& material_desc);
	int AddMaterialDescIfNotExists(const std::string& key, const MaterialDesc& material_desc);
	void DeleteMaterialDesc(int material_id)
	{
		// TODO: implement delete
	}
	MaterialDesc& GetMaterialDesc(int material_id);
	const MaterialDesc& GetMaterialDesc(int material_id) const;
private:
	ResourcePool<MaterialDesc> m_materials_pool{};
};

class RenderResource
{
public:
	void Initialize(
		ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swap_chain);
	void Finalize();
	void UpdateBufferPerFrame();
	void UpdateBufferPerView(const CameraViewData& view_data);
	// getters
	TextureLoader& GetTextureLoader() { return m_texture_loader; }
	ModelLoader& GetModelLoader() { return m_model_loader; }

	MaterialResourceManager& GetMaterialManager() { return m_material_manager; }
	const MaterialResourceManager& GetMaterialManager() const { return m_material_manager; }

	// constant buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_per_projection;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_per_projection_sprite;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_per_view;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_lights;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_per_mesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_per_screen_size;
	// Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_light_shadows;
	// Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_per_frame;

	// instancing
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_instancing;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_buffer_instancing_srv;

	// textures
	// const TextureResource2D& GetRenderTexture2D(int texture_id) const
	// {
	// 	return std::get<TextureResource2D>(m_render_textures_pool.Get(texture_id));
	// }
	// const TextureResourceCube& GetRenderTextureCube(int texture_id) const
	// {
	// 	return std::get<TextureResourceCube>(m_render_textures_pool.Get(texture_id));
	// }
	// TextureResource2D& GetRenderTexture2D(int texture_id)
	// {
	// 	return std::get<TextureResource2D>(m_render_textures_pool.Get(texture_id));
	// }
	// TextureResourceCube& GetRenderTextureCube(int texture_id)
	// {
	// 	return std::get<TextureResourceCube>(m_render_textures_pool.Get(texture_id));
	// }
	// int RegisterRenderTexture(
	// 	const std::string& key,
	// 	const std::variant<TextureResource2D, TextureResourceCube>& texture)
	// {
	// 	const int id = m_render_textures_pool.AddUnique(key, texture);
	// 	return id;
	// }
	// TextureResourceId GetRenderTextureId(const std::string& key) const
	// {
	// 	int texture_id = m_render_textures_pool.GetId(key);
	// 	TextureResourceId resource_id{};
	// 	resource_id.id = texture_id;
	// 	resource_id.type = TextureType::CAMERA;
	// 	return resource_id;
	// }
private:
	// resource loaders
	ModelLoader m_model_loader;
	TextureLoader m_texture_loader;

	MaterialResourceManager m_material_manager;

	// created textures and views
	ResourcePool<std::variant<TextureResource2D, TextureResourceCube> > m_render_textures_pool;

	// 注意！初期化で外部から設定されるもの。Release不要。
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};