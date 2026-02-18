#include "render_resource.h"

#include <DirectXMath.h>
#include "render/resource/buffer.h"

#include "render_common.h"
#include "global_context.h"
#include "render_system.h"
#include "render_scene.h"

#include "shader_setting.h"
#include "config/constant.h" // screen size // TODO

using namespace Microsoft::WRL;
using namespace DirectX;

void RenderResource::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swap_chain)
{
	m_device = device;
	m_context = context;
	// resource managers
	m_texture_loader.Initialize(m_device, m_context, swap_chain);
	m_model_loader.Initialize(m_device, m_context);

	// create common buffers?
	m_buffer_per_projection = Buffer::CreateConstantBuffer(m_device, sizeof(BufferPerProjection));
	m_buffer_per_projection_sprite = Buffer::CreateConstantBuffer(m_device, sizeof(BufferPerProjectionSprite));
	m_buffer_per_view = Buffer::CreateConstantBuffer(m_device, sizeof(BufferPerView));
	m_buffer_per_mesh = Buffer::CreateConstantBuffer(m_device, sizeof(BufferPerMesh));
	m_buffer_lights = Buffer::CreateConstantBuffer(m_device, sizeof(BufferLightScene));
	m_buffer_per_screen_size = Buffer::CreateConstantBuffer(m_device, sizeof(BufferScreenSize));
	//m_buffer_light_shadows = Buffer::CreateConstantBuffer(m_device, sizeof(BufferLightShadowScene));
	//m_buffer_per_frame = Buffer::CreateConstantBuffer(m_device, sizeof(BufferPerFrame));
	

	// instancing
	Buffer::CreateStructuredBuffer(
		m_device, sizeof(BufferPerMesh), MAX_INSTANCE_PER_DRAW_CALL,
		m_buffer_instancing, m_buffer_instancing_srv
	);
}

void RenderResource::Finalize()
{
	// using ComPtr, nothing to do here
}

void RenderResource::UpdateBufferPerFrame()
{
	auto& render_scene = g_global_context.m_render_system->GetRenderScene();
	// auto& camera = render_scene.m_camera_data_main;
	// XMMATRIX projection = XMLoadFloat4x4(&camera.mat_proj);
	// XMMATRIX view = XMLoadFloat4x4(&camera.mat_view);
	// // main camera projection // TODO update on main camera change??
	// {
	// 	// TODO
	// 	BufferPerProjection cb{};
	// 	XMStoreFloat4x4(&cb.matrix_proj, XMMatrixTranspose(projection));
	// 	XMStoreFloat4x4(&cb.matrix_proj_inverse, XMMatrixTranspose(XMMatrixInverse(nullptr, projection)));
	// 	m_context->UpdateSubresource(m_buffer_per_projection.Get(), 0, nullptr, &cb, 0, 0);
	// }
	// // main camera view
	// {
	// 	BufferPerView cb{};
	// 	XMMATRIX view_proj = view * projection;
	// 	XMStoreFloat4x4(&cb.matrix_view, XMMatrixTranspose(view));
	// 	XMStoreFloat4x4(&cb.matrix_view_inverse, XMMatrixTranspose(XMMatrixInverse(nullptr, view)));
	// 	XMStoreFloat4x4(&cb.matrix_view_proj, XMMatrixTranspose(view_proj));
	// 	XMStoreFloat4x4(&cb.matrix_view_proj_inverse, XMMatrixTranspose(XMMatrixInverse(nullptr, view_proj)));
	// 	cb.view_position_w = camera.position.ToXMFLOAT3(); // TODO
	// 	m_context->UpdateSubresource(m_buffer_per_view.Get(), 0, nullptr, &cb, 0, 0);
	// }
	// lights
	{
		BufferLightScene cb{ render_scene.m_lights };
		cb.dir_light.direction = g_shader_setting.light_direction.GetNormalized().ToXMFLOAT3();
		cb.dir_light.color = (g_shader_setting.light_color * g_shader_setting.light_intensity).ToXMFLOAT3();
		m_context->UpdateSubresource(m_buffer_lights.Get(), 0, nullptr, &cb, 0, 0);
	}
	// shadow mapping
	//{
	//	BufferLightShadowScene cb{};
	//	// TODO
	//	const auto target = Vector3{ 0.0f, 0.0f, 0.0f };
	//	const auto light_pos = g_shader_setting.light_direction.GetNormalized() * -100.0f;
	//	const auto up = Vector3{ 0.0f, 1.0f, 0.0f };

	//	XMMATRIX light_view = XMMatrixLookAtLH(
	//		XMVectorSet(light_pos.x, light_pos.y, light_pos.z, 1.0f),
	//		XMVectorSet(target.x, target.y, target.z, 1.0f),
	//		XMVectorSet(up.x, up.y, up.z, 0.0f)
	//	);

	//	float ortho_width = 300.0f;
	//	float ortho_height = 300.0f;
	//	float near_plane = 0.1f;
	//	float far_plane = 300.0f;
	//	XMMATRIX light_proj = XMMatrixOrthographicLH(ortho_width, ortho_height, near_plane, far_plane);

	//	XMStoreFloat4x4(&cb.dir_light_shadow.matrix_light_view_proj, XMMatrixTranspose(light_view * light_proj));
	//	m_context->UpdateSubresource(m_buffer_light_shadows.Get(), 0, nullptr, &cb, 0, 0);
	//}

	// BufferPerFrame cb{};
	// XMStoreFloat4x4(&cb.matrix_view_proj, XMMatrixTranspose(view * projection));
	// XMStoreFloat4x4(&cb.matrix_view, XMMatrixTranspose(view));
	// view.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // TODO: for skybox
	// XMStoreFloat4x4(&cb.matrix_view_proj_no_transform, XMMatrixTranspose(view * projection));

	// camera
	// const auto& camera_pos = camera.position;
	// cb.camera_position = DirectX::XMFLOAT3(camera_pos.x, camera_pos.y, camera_pos.z);
	// // light
	// cb.directional_light.direction = DirectX::XMFLOAT3(
	// 	g_shader_setting.light_direction.x,
	// 	g_shader_setting.light_direction.y,
	// 	g_shader_setting.light_direction.z);
	// // TODO: light matrix
	// {
	// 	// TODO
	// 	const auto target = Vector3{ 0.0f, 0.0f, 0.0f };
	// 	const auto light_pos = g_shader_setting.light_direction.GetNormalized() * -200.0f;
	// 	const auto up = Vector3{ 0.0f, 1.0f, 0.0f };
	// 
	// 	XMMATRIX light_view = XMMatrixLookAtLH(
	// 		XMVectorSet(light_pos.x, light_pos.y, light_pos.z, 1.0f),
	// 		XMVectorSet(target.x, target.y, target.z, 1.0f),
	// 		XMVectorSet(up.x, up.y, up.z, 0.0f)
	// 	);
	// 
	// 	float ortho_width = 300.0f;
	// 	float ortho_height = 300.0f;
	// 	float near_plane = 0.1f;
	// 	float far_plane = 300.0f;
	// 	XMMATRIX light_proj = XMMatrixOrthographicLH(ortho_width, ortho_height, near_plane, far_plane);
	// 
	// 	XMStoreFloat4x4(&cb.matrix_directional_light_view_proj,
	// 		XMMatrixTranspose(light_view * light_proj));
	// }

	// 定数バッファに行列をセット
	// m_context->UpdateSubresource(m_buffer_per_frame.Get(), 0, nullptr, &cb, 0, 0);
}

void RenderResource::UpdateBufferPerView(const CameraViewData& view_data)
{
	XMMATRIX projection = XMLoadFloat4x4(&view_data.mat_proj);
	XMMATRIX view = XMLoadFloat4x4(&view_data.mat_view);
	// main camera projection // TODO update on main camera change??
	{
		// TODO
		BufferPerProjection cb{};
		XMStoreFloat4x4(&cb.matrix_proj, XMMatrixTranspose(projection));
		XMStoreFloat4x4(&cb.matrix_proj_inverse, XMMatrixTranspose(XMMatrixInverse(nullptr, projection)));
		m_context->UpdateSubresource(m_buffer_per_projection.Get(), 0, nullptr, &cb, 0, 0);
	}
	// main camera view
	{
		BufferPerView cb{};
		XMMATRIX view_proj = view * projection;
		XMStoreFloat4x4(&cb.matrix_view, XMMatrixTranspose(view));
		XMStoreFloat4x4(&cb.matrix_view_inverse, XMMatrixTranspose(XMMatrixInverse(nullptr, view)));
		XMStoreFloat4x4(&cb.matrix_view_proj, XMMatrixTranspose(view_proj));
		XMStoreFloat4x4(&cb.matrix_view_proj_inverse, XMMatrixTranspose(XMMatrixInverse(nullptr, view_proj)));
		cb.view_position_w = view_data.position.ToXMFLOAT3(); // TODO
		m_context->UpdateSubresource(m_buffer_per_view.Get(), 0, nullptr, &cb, 0, 0);
	}
	if (view_data.has_sprite_projection)
	{
		XMMATRIX projection_sprite = XMLoadFloat4x4(&view_data.mat_proj_sprite);
		BufferPerProjectionSprite cb{};
		XMStoreFloat4x4(&cb.matrix_proj, XMMatrixTranspose(projection_sprite));
		m_context->UpdateSubresource(m_buffer_per_projection_sprite.Get(), 0, nullptr, &cb, 0, 0);
	}
	// screen size
	{
		BufferScreenSize cb{};
		cb.screen_width = view_data.screen_width;
		cb.screen_height = view_data.screen_height;
		m_context->UpdateSubresource(m_buffer_per_screen_size.Get(), 0, nullptr, &cb, 0, 0);
	}
	{
	}
}

// material resource manager start ====
int MaterialResourceManager::AddMaterialDescWithGeneratedKey(const MaterialDesc& material_desc)
{
	// add with generated key
	return m_materials_pool.AddUniqueWithGeneratedKey("", material_desc);
}

int MaterialResourceManager::AddMaterialDescIfNotExists(const std::string& key, const MaterialDesc& material_desc)
{
	// add if not exists, otherwise get existing id
	return m_materials_pool.AddIfNotExists(key, material_desc);
}

MaterialDesc& MaterialResourceManager::GetMaterialDesc(int material_id)
{
	return m_materials_pool.Get(material_id);
}

const MaterialDesc& MaterialResourceManager::GetMaterialDesc(int material_id) const
{
	return m_materials_pool.Get(material_id);
}
// material resource manager end ====
