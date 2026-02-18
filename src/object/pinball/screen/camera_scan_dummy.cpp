#include "camera_scan_dummy.h"
#include "object/game_object_list.h"
#include "object/common/projector.h"
#include "component/render/component_renderer_mesh.h"
#include "render/render_resource.h"
#include "render/resource/texture_loader.h"
#include "config/preset_manager.h"
#include "config/camera_names.h"


void CameraScanDummy::Initialize()
{
	m_projector = GetOwner().CreateGameObject<Projector>();
}

void CameraScanDummy::InitializeConfig()
{
	m_display_center = Vector3{0.0f, -40.0f, 0.0f};
	m_transform.SetPosition({ 0.0f, 40.0f, 0.0f });
	
	// TODO
	m_projector_shape.shape_type = CameraShapeType::PERSPECTIVE;
	m_projector_shape.fov = 0.5f;
	m_projector_shape.aspect_ratio = 1.0f;
	m_projector_shape.z_near = 20.0f;
	m_projector_shape.z_far = 100.0f;

	auto& texture_loader = GetTextureLoader();
	const auto screen_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.screen_main.name);
	// const auto screen_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/test_projector.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	// projector
	auto projector = m_projector.lock();
	projector->InitializeConfig(
		screen_texture_id,
		m_projector_shape,
		m_transform.GetPositionGlobal(),
		m_view_center,
		Vector3{ 0.0f, 0.0f, 1.0f }
	);
}

void CameraScanDummy::Update()
{
	//auto player = GetOwner().FindGameObject<Player>();
	//m_view_center = player->GetTransform().GetPositionGlobal();
}

CameraScanDummy::ScanInfo CameraScanDummy::GetDummyWorldInfo() const
{
	CameraScanDummy::ScanInfo scan_info{};
	scan_info.view_center = m_view_center;
	scan_info.display_center = m_display_center;
	scan_info.rotation_y = m_rotation_y;
	scan_info.scale_factor = 0.02f; // TODO: config
	return scan_info;
}

void CameraScanDummy::SetViewCenter(const Vector3& view_center, float rotation_y)
{
	m_view_center = view_center;
	m_rotation_y = rotation_y;
	UpdateFollow();
}

void CameraScanDummy::UpdateFollow()
{
	// update projector
	Vector3 position = m_view_center + Vector3{ 0.0f, 40.0f, 0.0f };
	Vector3 up = m_display_center - m_view_center;
	up.y = 0.0f;

	// projector
	auto projector = m_projector.lock();
	projector->UpdateShape(
		m_projector_shape,
		position,
		m_view_center,
		up
	);
}
