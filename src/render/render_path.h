#pragma once
#include <vector>
#include <memory>
#include "render/directx.h"
#include "render/resource/view_context.h"
#include "render/render_camera.h"
#include "editor/editor_item.h"

struct RenderPathViewContext
{
	CameraRenderLayer render_layer;
	bool invert_culling{ false };
	class RenderCameraBase* camera{ nullptr };
};

class RenderPathBase
{
public:
	virtual ~RenderPathBase() = default;
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void InitializeViewContext(RenderViewKey view_id, uint32_t width, uint32_t height) = 0;
	virtual void UpdateViewContext(const RenderPathViewContext& view_context) = 0;
	virtual void Draw(RenderViewKey view_key, const ViewContext& view_context) = 0;
	// editor
	virtual void GetEditorItems(RenderViewKey view_key, std::vector<EditorItem>&items) const {};
	// view context
	const RenderPathViewContext& GetViewContext() const { return m_view_context; }
	void SetViewContext(const RenderPathViewContext& view_context) { m_view_context = view_context; }
protected:
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	RenderPathViewContext m_view_context{};
};

class RenderPathManager
{
public:
	virtual ~RenderPathManager() = default;
	virtual void Initialize();
	size_t AddPath(std::unique_ptr<RenderPathBase> path);
	void InitializePathResource(const CameraUsageConfig& config);
	void Draw(const class RenderScene& render_scene);
	void Finalize();
	RenderPathBase& GetRenderPath(size_t path_index);
	RenderPathBase& GetActiveRenderPath();
	size_t GetRenderPathNum() const { return m_paths.size(); }
private:
	std::string GetViewContextKey(const CameraUsageConfig& config);
	std::vector<std::unique_ptr<RenderPathBase> > m_paths{};
	int m_active_path_index{ -1 };
};