#include "render/render_camera.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/resource/texture_resource.h"
#include "render/render_common.h"
#include "render/render_path.h"
#include "platform/window.h"

using namespace DirectX;

// base start ========
ID3D11Device* RenderCameraBase::GetDevice()
{
	return g_global_context.m_render_system->GetDevice();
}
IDXGISwapChain* RenderCameraBase::GetSwapChain()
{
	return g_global_context.m_render_system->GetSwapChain();
}
// base end ========

// render camera start ========
void RenderCamera::Initialize(const CameraUsageConfig& usage)
{
	// TODO: validation
	m_active = true;
	m_usage = usage;
	// create textures
	InitializeTextures();
}

void RenderCamera::Finalize()
{
	// release textures
	ReleaseTextures();
}

// draw
DirectX::XMMATRIX RenderCamera::GetProjectionMatrix(int index)
{
	assert(index == 0);
	switch (m_shape.shape_type)
	{
	case CameraShapeType::PERSPECTIVE:
	{
		return CameraMath::CalculateProjectionMatrixPerspective(
			m_shape.fov, m_shape.aspect_ratio, m_shape.z_near, m_shape.z_far);
	}
	case CameraShapeType::ORTHOGRAPHIC:
	{
		return CameraMath::CalculateProjectionMatrixOrthographic(
			m_usage.width, m_usage.height, m_shape.z_near, m_shape.z_far);
	}
	default:
	{
		assert(false); // unsupported
	}
	}
}

DirectX::XMMATRIX RenderCamera::GetViewMatrix(int index)
{
	assert(index == 0);
	return CameraMath::CalculateViewMatrix(m_position, m_target, m_up);
}

bool RenderCamera::GetProjectionMatrixSprite(DirectX::XMMATRIX& mat) const
{
	switch (m_usage.type)
	{
	case CameraType::CAMERA_MAIN:
	case CameraType::CAMERA:
	{
		// TODO: same size on different resolution?
		mat = CameraMath::CalculateProjectionMatrixOrthographic(
			m_usage.width, m_usage.height, 0.0f, 1.0f);
		return true;
	}
	default:
	{
		return false;
	}
	}
}

Vector3 RenderCamera::GetPosition(int index)
{
	return m_position;
}

RenderTarget RenderCamera::GetRenderTarget(int index)
{
	assert(index == 0);
	assert(m_render_target_texture_id.IfValid() && m_depth_stencil_texture_id.IfValid());

	RenderTarget rt{};
	rt.Initialize(m_usage.width, m_usage.height);
	if (m_usage.type == CameraType::CAMERA_MAIN)
	{
		rt.AdjustViewPort(g_global_context.m_window->GetWidth(), g_global_context.m_window->GetHeight());
	}
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	const auto rtv = texture_loader.GetRenderTextureRenderTargetView(m_render_target_texture_id);
	const auto dsv = texture_loader.GetRenderTextureDepthStencilView(m_depth_stencil_texture_id);
	rt.AddRenderTarget(rtv);
	rt.SetDepthStencil(dsv);
	return rt;
}

void RenderCamera::AfterDraw()
{
	SwapTextures();
}

// camera features
void RenderCamera::Resize(uint32_t width, uint32_t height)
{
	m_usage.width = width;
	m_usage.height = height;
	ResizeTextures();
}

void RenderCamera::SetShape(const CameraShapeConfig& shape)
{
	// TODO: validation
	m_shape = shape;
}
void RenderCamera::SetTransform(const Vector3& position, const Vector3& target, const Vector3& up)
{
	m_position = position;
	m_target = target;
	m_up = up;
}

// render camera utils...
void RenderCamera::InitializeTextures()
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();

	// output textures
	// render target
	{
		const auto texture_key = GetRenderTargetKey(m_usage.render_camera_key);
		const auto texture_usage = m_usage.type == CameraType::CAMERA_MAIN
			? TextureUsageType::BACK_BUFFER : TextureUsageType::RENDER_TARGET;
		auto desc = TextureResource2D::DESC_PREST_RENDER_TARGET;
		desc.Width = m_usage.width;
		desc.Height = m_usage.height;
		// all render targets are in linear space, except that
		// back buffer is in SRGB, for automatic conversion from linear to SRGB
		desc.Format = m_usage.type == CameraType::CAMERA_MAIN
			? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		m_render_target_texture_id = texture_loader.GetOrCreateRenderTexture2D(
			texture_key,
			texture_usage,
			desc
		);
	}
	// depth stencil
	{
		const auto texture_key = GetDepthStencilKey(m_usage.render_camera_key);
		const auto desc = TextureResource::BuildTexturesDesc(
			TextureResource2D::DESC_PREST_DEPTH_STENCIL, m_usage.width, m_usage.height);
		m_depth_stencil_texture_id = texture_loader.GetOrCreateRenderTexture2D(
			texture_key,
			TextureUsageType::DEPTH_STENCIL,
			desc
		);
	}
}

void RenderCamera::ResizeTextures() const
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	// render target
	if (m_render_target_texture_id.IfValid())
	{
		texture_loader.ResizeRenderTexture(m_render_target_texture_id, m_usage.width, m_usage.height);
	}
	// depth stencil
	if (m_depth_stencil_texture_id.IfValid())
	{
		texture_loader.ResizeRenderTexture(m_depth_stencil_texture_id, m_usage.width, m_usage.height);
	}
	auto& render_path_manager = g_global_context.m_render_system->GetRenderPathManager();
	auto& render_path = render_path_manager.GetRenderPath(m_usage.render_path_id);

}

void RenderCamera::ReleaseTextures()
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	// render target
	if (m_render_target_texture_id.IfValid())
	{
		texture_loader.ReleaseRenderTexture(m_render_target_texture_id);
	}
	// depth stencil
	if (m_depth_stencil_texture_id.IfValid())
	{
		texture_loader.ReleaseRenderTexture(m_depth_stencil_texture_id);
	}
}
void RenderCamera::SwapTextures() const
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	// render target
	if (m_render_target_texture_id.IfValid())
	{
		texture_loader.SwapRenderTexture(m_render_target_texture_id);
	}
	// depth stencil
	if (m_depth_stencil_texture_id.IfValid())
	{
		texture_loader.SwapRenderTexture(m_depth_stencil_texture_id);
	}
}
// render camera utils... end

// render camera end ========

// cube canera start ========
void RenderCameraCube::Initialize(const CameraUsageConfig& usage)
{
	// TODO: validation
	m_active = true;
	m_usage = usage;
	// create textures
	InitializeTextures();
}

void RenderCameraCube::Finalize()
{
	// release textures
	ReleaseTextures();
}

// draw
DirectX::XMMATRIX RenderCameraCube::GetProjectionMatrix(int index)
{
	assert(index >= 0 && index <= 6);
	return CameraMath::CalculateProjectionMatrixCube(m_shape.z_near, m_shape.z_far);
}

DirectX::XMMATRIX RenderCameraCube::GetViewMatrix(int index)
{
	assert(index >= 0 && index <= 6);
	return CameraMath::CalculateViewMatrixCube(index, m_position);
}

Vector3 RenderCameraCube::GetPosition(int index)
{
	return m_position;
}

RenderTarget RenderCameraCube::GetRenderTarget(int index)
{
	assert(index >= 0 && index <= 6);
	assert(m_render_target_texture_id.IfValid() && m_depth_stencil_texture_id.IfValid());

	RenderTarget rt{};
	rt.Initialize(m_usage.width, m_usage.height);

	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	const auto rtv = texture_loader.GetRenderTextureRenderTargetView(m_render_target_texture_id, index);
	const auto dsv = texture_loader.GetRenderTextureDepthStencilView(m_depth_stencil_texture_id);
	rt.AddRenderTarget(rtv);
	rt.SetDepthStencil(dsv);
	return rt;
}

void RenderCameraCube::AfterDraw()
{
	SwapTextures();
}

// camera features
void RenderCameraCube::Resize(uint32_t width, uint32_t height)
{
	m_usage.width = width;
	m_usage.height = height;
	ResizeTextures();
}

void RenderCameraCube::SetShape(const CameraShapeConfig& shape)
{
	// TODO: validation
	m_shape = shape;
}
void RenderCameraCube::SetTransform(const Vector3& position, const Vector3& target, const Vector3& up)
{
	m_position = position;
	// cube camera computes target and up internally
}

// render camera utils...
void RenderCameraCube::InitializeTextures()
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();

	// output textures
	// render target
	{
		const auto texture_key = GetRenderTargetKey(m_usage.render_camera_key);
		const auto texture_usage = m_usage.type == CameraType::CAMERA_MAIN
			? TextureUsageType::BACK_BUFFER : TextureUsageType::RENDER_TARGET;
		const auto desc = TextureResource::BuildTexturesDesc(
			TextureResourceCube::DESC_PREST_CUBE_TARGET, m_usage.width, m_usage.height);

		m_render_target_texture_id = texture_loader.GetOrCreateRenderTextureCube(
			texture_key,
			texture_usage,
			desc
		);
	}
	// depth stencil
	{
		const auto texture_key = GetDepthStencilKey(m_usage.render_camera_key);
		const auto desc = TextureResource::BuildTexturesDesc(
			TextureResource2D::DESC_PREST_DEPTH_STENCIL, m_usage.width, m_usage.height);
		m_depth_stencil_texture_id = texture_loader.GetOrCreateRenderTexture2D(
			texture_key,
			TextureUsageType::DEPTH_STENCIL,
			desc
		);
	}
}

void RenderCameraCube::ResizeTextures() const
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	// render target
	if (m_render_target_texture_id.IfValid())
	{
		texture_loader.ResizeRenderTexture(m_render_target_texture_id, m_usage.width, m_usage.height);
	}
	// depth stencil
	if (m_depth_stencil_texture_id.IfValid())
	{
		texture_loader.ResizeRenderTexture(m_depth_stencil_texture_id, m_usage.width, m_usage.height);
	}
}

void RenderCameraCube::ReleaseTextures()
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	// render target
	if (m_render_target_texture_id.IfValid())
	{
		texture_loader.ReleaseRenderTexture(m_render_target_texture_id);
	}
	// depth stencil
	if (m_depth_stencil_texture_id.IfValid())
	{
		texture_loader.ReleaseRenderTexture(m_depth_stencil_texture_id);
	}
}
void RenderCameraCube::SwapTextures() const
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	// render target
	if (m_render_target_texture_id.IfValid())
	{
		texture_loader.SwapRenderTexture(m_render_target_texture_id);
	}
	// depth stencil
	if (m_depth_stencil_texture_id.IfValid())
	{
		texture_loader.SwapRenderTexture(m_depth_stencil_texture_id);
	}
}
// cube camera end ========


bool SceneCameraManager::CreateCameraMainIfNotExists(const CameraUsageConfig& config)
{
	// TODO: config validation
	assert(config.type == CameraType::CAMERA_MAIN);
	// ?
	const auto& key = config.render_camera_key;
	auto it = m_cameras.find(key);
	if (it != m_cameras.end())
	{
		// TODO
		return false;
	}
	auto camera = std::make_unique<RenderCamera>();
	camera->Initialize(config);
	m_cameras.emplace(key, std::move(camera));

	return true;
}

bool SceneCameraManager::CreateCamera(const CameraUsageConfig& config)
{
	// TODO: config validation
	assert(config.type == CameraType::CAMERA);

	const auto& key = config.render_camera_key;
	auto it = m_cameras.find(key);
	// error: already exists
	assert(it == m_cameras.end());

	auto camera = std::make_unique<RenderCamera>();
	camera->Initialize(config);
	m_cameras.emplace(key, std::move(camera));
	return true;
}

bool SceneCameraManager::CreateCameraReflect(const CameraUsageConfig& config)
{
	// TODO: config validation
	assert(config.type == CameraType::REFLECTION);

	const auto& key = config.render_camera_key;
	auto it = m_cameras.find(key);
	// error: already exists
	assert(it == m_cameras.end());

	auto camera = std::make_unique<RenderCameraCube>();
	camera->Initialize(config);
	m_cameras.emplace(key, std::move(camera));
	return true;
}

void SceneCameraManager::Update(const CameraSwapData& camera_swap_data)
{
	// add new cameras (before updates! for new cameras also needs to be updated)
	for (const auto& camera_config : camera_swap_data.cameras_to_add)
	{
		bool created{ false };
		switch (camera_config.type)
		{
		case CameraType::CAMERA_MAIN:
		{
			created = CreateCameraMainIfNotExists(camera_config);
			break;
		}
		case CameraType::CAMERA:
		{
			created = CreateCamera(camera_config);
			break;
		}
		case CameraType::REFLECTION:
		{
			created = CreateCameraReflect(camera_config);
			break;
		}
		}

		if (created)
		{
			auto& render_path_manager = g_global_context.m_render_system->GetRenderPathManager();
			render_path_manager.InitializePathResource(camera_config);
		}
	}
	// update cameras
	for (auto& camera : m_cameras)
	{
		camera.second->SetActive(false);
	}
	for (const auto& camera_update_data : camera_swap_data.cameras_to_update)
	{
		const auto& camera_config = camera_update_data.config;

		auto it = m_cameras.find(camera_config.render_camera_key);
		assert(it != m_cameras.end());
		auto& camera = it->second;
		camera->SetShape(camera_update_data.shape);
		camera->SetTransform(
			camera_update_data.position,
			camera_update_data.target,
			camera_update_data.up
		);
		camera->SetActive(true);
	}
	// delete
	for (const auto& camera_config : camera_swap_data.cameras_to_remove)
	{
		CameraType type = camera_config.type;
		if (type == CameraType::CAMERA_MAIN)
		{
			// do not delete main camera
			continue;
		}
		const auto& key = camera_config.render_camera_key;
		auto it = m_cameras.find(key);
		if (it != m_cameras.end())
		{
			it->second->Finalize();
			m_cameras.erase(it);
		}
	}
}

std::vector<RenderCameraBase*> SceneCameraManager::GetActiveCameras() const
{
	std::vector<RenderCameraBase*> cameras_all{};
	for (auto& camera : m_cameras)
	{
		if (camera.second->GetActive())
		{
			cameras_all.push_back(camera.second.get());
		}
	}

	std::sort(cameras_all.begin(), cameras_all.end(),
		[](const RenderCameraBase* a, const RenderCameraBase* b) {
			return a->GetUsageConfig().render_order < b->GetUsageConfig().render_order;
		});
	return cameras_all;
}

void SceneCameraManager::ResizeMainCamera(uint32_t width, uint32_t height)
{
	for (auto& camera : m_cameras)
	{
		if (camera.second->GetUsageConfig().type == CameraType::CAMERA_MAIN)
		{
			camera.second->Resize(width, height);
		}
	}
}