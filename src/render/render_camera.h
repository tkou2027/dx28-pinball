#pragma once
#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "math/vector3.h"
#include "math/camera_math.h"
#include "render/config/camera_data.h"
#include "render/config/texture_resource_id.h"

#include "render/resource/view_context.h"
#include "render/render_swap_data.h"
#include "editor/editor_item.h"

typedef std::string RenderViewKey;

class RenderCameraBase
{
public:
	virtual ~RenderCameraBase() = default;
	// life cycle
	virtual void Initialize(const CameraUsageConfig& usage) = 0;
	virtual void Update() = 0;
	virtual void Finalize() = 0;
	// draw
	virtual int GetViewContextCount() const = 0;
	virtual DirectX::XMMATRIX GetProjectionMatrix(int index) const = 0;
	virtual DirectX::XMMATRIX GetViewMatrix(int index) const = 0;
	// some cameras have projection matrix for sprite rendering
	virtual bool GetProjectionMatrixSprite(DirectX::XMMATRIX& mat) const { return false; }
	virtual Vector3 GetPosition(int index) = 0;
	virtual RenderTarget GetRenderTarget(int index) = 0;
	// after draw
	virtual void AfterDraw() = 0;
	// shape
	virtual void SetShape(const CameraShapeConfig& shape) = 0; // projection
	virtual void SetTransform(const CameraTransformData& transform) = 0; // camera
	virtual AABB GetFrustumBoundingBox() const { return AABB::universe; }

	virtual void Resize(uint32_t width, uint32_t height) = 0;
	// virtual void GetSize(uint32_t& width, uint32_t& height) = 0;
	// virtual void SetShape(const CameraShapeConfig& shape) = 0;
	void SetActive(bool is_active) { m_active = is_active; }
	bool GetActive() const { return m_active; }
	void SetRemoved(bool is_removed) { m_removed = is_removed; }
	bool GetRemoved() const { return m_removed; }
	const CameraUsageConfig& GetUsageConfig() const { return m_usage; }
	// editor
	virtual void GetEditorItems(std::vector<EditorItem>& items) const;
	virtual ID3D11ShaderResourceView* GetShaderResourceView(int index) const = 0;
	virtual ID3D11ShaderResourceView* GetShaderResourceViewDepthStencil(int index) const = 0;
protected:
	bool m_active{ false };
	bool m_removed{ false };
	CameraUsageConfig m_usage{};


	static std::string GetRenderTargetKey(const std::string& render_texture_key) { return render_texture_key; }
	static std::string GetDepthStencilKey(const std::string& render_texture_key) { return render_texture_key + "_depth_stencil"; }
	static ID3D11Device* GetDevice();
	static IDXGISwapChain* GetSwapChain();
};

class RenderCamera : public RenderCameraBase
{
public:
	void Initialize(const CameraUsageConfig& usage) override;
	void Update() override; // compute matrices
	void Finalize() override;

	int GetViewContextCount() const override { return 1; }
	DirectX::XMMATRIX GetProjectionMatrix(int index) const override;
	DirectX::XMMATRIX GetViewMatrix(int index) const override;
	bool GetProjectionMatrixSprite(DirectX::XMMATRIX& mat) const override;
	Vector3 GetPosition(int index) override;
	RenderTarget GetRenderTarget(int index) override;
	AABB GetFrustumBoundingBox() const override;

	void AfterDraw() override;

	void Resize(uint32_t width, uint32_t height);
	void SetShape(const CameraShapeConfig& shape) override; // projection
	void SetTransform(const CameraTransformData& transform) override; // camera
	// editor / render
	ID3D11ShaderResourceView* GetShaderResourceView(int index) const;
	ID3D11ShaderResourceView* GetShaderResourceViewDepthStencil(int index) const;
private:
	void InitializeTextures();
	void ResizeTextures() const;
	void ReleaseTextures();
	void SwapTextures() const;
	DirectX::XMMATRIX ComputeViewMatrix() const;
	DirectX::XMMATRIX ComputeProjectionMatrix() const;


	// camera states
	CameraShapeConfig m_shape{};
	Vector3 m_position{};
	Vector3 m_up{ 0.0f, 1.0f, 0.0f };
	Vector3 m_target{ 0.0f, 5.0f, 0.0f };
	CameraPlaneReflectionConfig m_reflection_plane_config{};

	TextureResourceId m_render_target_texture_id{};
	TextureResourceId m_depth_stencil_texture_id{};

	DirectX::XMFLOAT4X4 m_projection_matrix{};
	DirectX::XMFLOAT4X4 m_view_matrix{};
};

class RenderCameraCube : public RenderCameraBase
{
public:
	void Initialize(const CameraUsageConfig& usage) override;
	void Update() override {} // TODO cache matrices
	void Finalize() override;

	int GetViewContextCount() const override { return 6; }
	DirectX::XMMATRIX GetProjectionMatrix(int index) const override;
	DirectX::XMMATRIX GetViewMatrix(int index) const override;
	Vector3 GetPosition(int index) override;
	RenderTarget GetRenderTarget(int index) override;
	void AfterDraw() override;

	void Resize(uint32_t width, uint32_t height);
	void SetShape(const CameraShapeConfig& shape) override; // projection
	void SetTransform(const CameraTransformData& transform) override; // camera

	// editor
	ID3D11ShaderResourceView* GetShaderResourceView(int index) const;
	ID3D11ShaderResourceView* GetShaderResourceViewDepthStencil(int index) const { return nullptr; } // TODO
private:
	void InitializeTextures();
	void ResizeTextures() const;
	void ReleaseTextures();
	void SwapTextures() const;


	// camera states
	CameraShapeConfig m_shape{};
	Vector3 m_position{};

	TextureResourceId m_render_target_texture_id{};
	TextureResourceId m_depth_stencil_texture_id{};
	// for editor
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_per_face_srv[6]{};
};

class SceneCameraManager
{
public:
	bool CreateCameraMainIfNotExists(const CameraUsageConfig& config);
	bool CreateCamera(const CameraUsageConfig& config);
	bool CreateCameraReflect(const CameraUsageConfig& config);
	void Update(const CameraSwapData& camera_swap_data);
	void UpdateRelease(const CameraSwapData& camera_swap_data);
	std::vector<RenderCameraBase*> GetActiveCameras() const;
	std::vector<RenderCameraBase*> GetAllCameras() const;
	std::vector<RenderCameraBase*> GetActiveCamerasOfType(CameraType type) const;
	void ResizeMainCamera(uint32_t width, uint32_t height);
private:
	void InitializePathResource(const CameraUsageConfig& config);
	void SortCameras(std::vector<RenderCameraBase*>& cameras) const;
	std::unordered_map<std::string, std::unique_ptr<RenderCameraBase> > m_cameras{};
};