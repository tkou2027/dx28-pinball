#pragma once
#include <vector>
#include <DirectXMath.h>
#include "render/config/model_desc.h"
#include "render/config/sprite_desc.h"
#include "render/config/model_state.h"
#include "render/config/camera_data.h"
#include "render/config/camera_render_layer.h"
#include "render/render_common.h"
#include "render/render_camera.h"
#include "render/particle/texture_particle_item.h"
#include "render/resource/billboard_data.h"

class SceneRenderablesManager
{
public:
	// data per view
	struct LayerRenderables
	{
		// meshes
		// std::vector<size_t> indices_static;
		// std::vector<size_t> indices_skinned;
		// std::vector<size_t> indices_instanced;
		std::vector<size_t> indices_model;
		// sprites
		std::vector<size_t> indices_sprite;
	};
	void Update();
	const LayerRenderables& GetRenderablesOfLayer(CameraRenderLayer layer) const
	{
		return m_view_renderables[static_cast<size_t>(layer)];
	}
public:
	// data
	// std::vector<ModelRenderInfo> m_models_static;
	// std::vector<ModelRenderInfo> m_models_skinned;
	// std::vector<ModelRenderInfo> m_models_instanced;

	std::vector<ModelRenderInfo> m_models;
	std::vector<SpriteOutput> m_sprites;
	std::vector<TextureParticleItem> m_particles;

private:
	std::array<LayerRenderables, static_cast<size_t>(CameraRenderLayer::MAX)> m_view_renderables{};
private:
	// enum class ModelType
	// {
	// 	STATIC,
	// 	SKINNED,
	// 	INSTANCED
	// };
	void Reset();
	void UpdateSceneData();
	void UpdateViewData();
	// utils
	void ProcessModelData(std::vector<Model>& models);
	// std::vector<ModelRenderInfo>& GetModelsOfType(ModelType model_type);
	// void ProcessModelDataToLayers(ModelType model_type);	
};

class RenderScene
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	void Finalize();
	void Update();
	SceneCameraManager& GetCameraManager() { return m_camera_manager; }
	const SceneCameraManager& GetCameraManager() const { return m_camera_manager; }
	SceneRenderablesManager& GetRenderablesManager() { return m_renderables; }
	const SceneRenderablesManager& GetRenderablesManager() const { return m_renderables; }
private:
	void UpdateCameras();
	void UpdateLights();
	void UpdateObjects();
	void UpdateParticles();
	void ProcessModelData(std::vector<Model>& models);
	// lighting
public:
	// objects (TODO: visible)
	std::vector<ModelRenderInfo> m_objects_main;
	std::vector<ModelRenderInfo> m_objects_main_skinned;
 	std::vector<ModelRenderInfo> m_objects_light_direction;
	std::vector<SpriteOutput> m_sprites;
	std::vector<TextureParticleItem> m_particles;
	// lights
	BufferLightScene m_lights;
	std::vector<ModelRenderInfo> m_light_models;
	// billboards
	BillboardData m_billboards;
private:
	SceneCameraManager m_camera_manager{};
	SceneRenderablesManager m_renderables{};
};