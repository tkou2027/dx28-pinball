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
		std::vector<size_t> indices_model;
		// sprites
		std::vector<size_t> indices_sprite;
	};
	struct ModelBoundingBoxData
	{
		AABB bbox;
		// radius...
	};
	void Update();
	const LayerRenderables& GetRenderablesOfLayer(CameraRenderLayer layer) const
	{
		return m_view_renderables[static_cast<size_t>(layer)];
	}
	bool GetModelsOfCamera(class RenderCameraBase* camera, std::vector<size_t>& out_indices_model) const;
public:
	std::vector<ModelRenderInfo> m_models;
	std::vector<SpriteOutput> m_sprites;
	std::vector<ParticleEmitter> m_particles;
	std::vector<ModelBoundingBoxData> m_bounding_boxes;
private:
	std::array<LayerRenderables, static_cast<size_t>(CameraRenderLayer::MAX)> m_view_renderables{};
private:
	void Reset();
	void UpdateSceneData();
	void UpdateViewData();
	// utils
	void ProcessModelData(std::vector<Model>& models);
};

class RenderScene
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	void Finalize();
	void Update();
	void UpdateRelease();
	SceneCameraManager& GetCameraManager() { return m_camera_manager; }
	const SceneCameraManager& GetCameraManager() const { return m_camera_manager; }
	SceneRenderablesManager& GetRenderablesManager() { return m_renderables; }
	const SceneRenderablesManager& GetRenderablesManager() const { return m_renderables; }
private:
	void UpdateCameras();
	void UpdateLights();
	void UpdateObjects();
	void ProcessModelData(std::vector<Model>& models);
	// lighting
public:
	// objects (TODO: visible)
	std::vector<ModelRenderInfo> m_objects_main;
	std::vector<ModelRenderInfo> m_objects_main_skinned;
 	std::vector<ModelRenderInfo> m_objects_light_direction;
	std::vector<SpriteOutput> m_sprites;
	// lights
	BufferLightScene m_lights;
	std::vector<ModelRenderInfo> m_light_models;
	// billboards
	BillboardData m_billboards;
private:
	SceneCameraManager m_camera_manager{};
	SceneRenderablesManager m_renderables{};
};