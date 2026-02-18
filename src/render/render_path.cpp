#include "render/render_path.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_scene.h"
#include "render/render_resource.h"

#include "render/path/render_path_main.h"
#include "render/path/render_path_monitor.h"
#include "render/path/render_path_screen_reflect.h"

ID3D11Device* RenderPathBase::GetDevice() const
{
	return g_global_context.m_render_system->GetDevice();
}

ID3D11DeviceContext* RenderPathBase::GetDeviceContext() const
{
	return g_global_context.m_render_system->GetDeviceContext();
}

void RenderPathManager::Initialize()
{
	//auto path_main = std::make_unique<RenderPathMain>();
	//path_main->Initialize();
	//m_paths.push_back(std::move(path_main));

	//auto path_monitor = std::make_unique<RenderPathMonitor>();
	//path_monitor->Initialize();
	//m_paths.push_back(std::move(path_monitor));

	//auto path_screen_reflect = std::make_unique<RenderPathScreenReflect>();
	//path_screen_reflect->Initialize();
	//m_paths.push_back(std::move(path_screen_reflect));
}

size_t RenderPathManager::AddPath(std::unique_ptr<RenderPathBase> path)
{
	const auto index = m_paths.size();
	path->Initialize();
	m_paths.push_back(std::move(path));
	return index;
}

void RenderPathManager::InitializePathResource(const CameraUsageConfig& config)
{
	RenderViewKey key = GetViewContextKey(config); // TODO
	auto& render_path = GetRenderPath(config.render_path_id);
	render_path.InitializeViewContext(
		key,
		config.width,
		config.height
	);
}

void RenderPathManager::Draw(const RenderScene& render_scene)
{
	// draw for each camera
	std::vector<RenderCameraBase*> cameras = render_scene.GetCameraManager().GetActiveCameras();
	auto& render_resource = g_global_context.m_render_system->GetRenderResource();
	for (RenderCameraBase* camera : cameras)
	{
		// get path used by camera
		const CameraUsageConfig& usage = camera->GetUsageConfig();
		const int path_id = usage.render_path_id;
		auto& path = m_paths.at(static_cast<size_t>(path_id));

		// update objects visible to camera
		// TODO: visible objects per frustrum
		const auto& camera_render_layer = usage.render_layer;
		path->UpdateVisibleRenderables(
			render_scene.GetRenderablesManager(),
			camera_render_layer
		);

		// camera might draw multiple times with different view & projection (e.g. cube)
		for (int i = 0; i < camera->GetViewContextCount(); i++)
		{
			// update view and projection matrix
			CameraViewData camera_view_data{};
			XMStoreFloat4x4(&camera_view_data.mat_proj, camera->GetProjectionMatrix(i));
			XMStoreFloat4x4(&camera_view_data.mat_view, camera->GetViewMatrix(i));
			camera_view_data.position = camera->GetPosition(i);
			DirectX::XMMATRIX mat_proj_sprite{};
			camera_view_data.has_sprite_projection = camera->GetProjectionMatrixSprite(mat_proj_sprite);
			if (camera_view_data.has_sprite_projection)
			{
				XMStoreFloat4x4(&camera_view_data.mat_proj_sprite, mat_proj_sprite);
			}
			camera_view_data.screen_width = static_cast<float>(camera->GetUsageConfig().width);
			camera_view_data.screen_height = static_cast<float>(camera->GetUsageConfig().height);
			render_resource.UpdateBufferPerView(camera_view_data);

			// TODO: visible objects
			ViewContext view_context{};
			view_context.render_target_out = camera->GetRenderTarget(i);
			RenderViewKey key = GetViewContextKey(camera->GetUsageConfig()); // TODO

			path->Draw(key, view_context);
		}
		// swap textures
		camera->AfterDraw();
	}
}

RenderPathBase& RenderPathManager::GetRenderPath(size_t path_index)
{
	return *m_paths.at(path_index);
}

std::string RenderPathManager::GetViewContextKey(const CameraUsageConfig& config)
{
	return config.render_camera_key;
	// TODO: share texture across cameras?
}
