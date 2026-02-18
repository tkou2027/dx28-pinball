#pragma once
#include <vector>
#include <memory>
#include "render/directx.h"
#include "render/resource/view_context.h"
#include "render/render_camera.h"

class RenderPathBase
{
public:
	virtual ~RenderPathBase() = default;
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void InitializeViewContext(RenderViewKey view_id, uint32_t width, uint32_t height) = 0;
	virtual void UpdateVisibleRenderables(
		const class SceneRenderablesManager& renderables,
		CameraRenderLayer render_layer
	) = 0;
	virtual void Draw(RenderViewKey view_key, const ViewContext& view_context) = 0;
protected:
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
};

class RenderPathManager
{
public:
	virtual ~RenderPathManager() = default;
	virtual void Initialize();
	size_t AddPath(std::unique_ptr<RenderPathBase> path);
	void InitializePathResource(const CameraUsageConfig& config);
	void Draw(std::vector<RenderCamera*> cameras);
	void Draw(const class RenderScene& render_scene);
	void Finalize();
	RenderPathBase& GetRenderPath(size_t path_index);
private:
	std::string GetViewContextKey(const CameraUsageConfig& config);
	std::vector<std::unique_ptr<RenderPathBase> > m_paths{};
};