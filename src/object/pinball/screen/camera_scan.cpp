#include "camera_scan.h"
#include "component/render/component_camera.h"
#include "config/camera_names.h"
#include "math/camera_math.h"
#include "object/game_object_list.h"
#include "object/pinball/screen/camera_scan_dummy.h"



using namespace DirectX;

void CameraScan::Initialize()
{
	auto& comp_camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	CameraUsageConfig config{};
	config.type = CameraType::CAMERA;
	auto& camera_preset = g_camera_presets.monitor;
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;

	CameraShapeConfig shape{};
	shape.aspect_ratio = static_cast<float>(config.width) / static_cast<float>(config.height);// 0.25f;

	comp_camera.InitializeCamera(config, shape);
	m_transform.SetPosition({ 0.0f, 10.0f, -10.0f });
	comp_camera.SetTarget({ 0.0f, 0.0f, 0.0f });
	// comp_camera.SetActive(false);
}

void CameraScan::SetCameraMain(std::weak_ptr<class GameObject> camera_main_weak)
{
	m_camera_main = camera_main_weak;
	auto camera_main = m_camera_main.lock();
	m_comp_id_reference_camera_main =
		camera_main->GetComponents().TryGetComponent<ComponentCamera>();
}

void CameraScan::SetReference(std::weak_ptr<class CameraScanDummy> scan_dummy)
{
	m_scan_dummy = scan_dummy;
}

void CameraScan::Update()
{
	//auto player = GetOwner().FindGameObject<Player>();
	//m_transform.SetPosition(player->GetTransform().GetPositionGlobal() + Vector3{ 0.0f, 5.0f, 5.0f });

	//auto& comp_camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	//comp_camera.SetTarget(player->GetTransform().GetPositionGlobal());

	auto& comp_camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	auto scan_dummy = m_scan_dummy.lock();
	auto camera_main = m_camera_main.lock();
	if (!scan_dummy || !camera_main)
	{
		// comp_camera.SetActive(false);
		return;
	}
	// comp_camera.SetActive(true);
	// dummy space
	auto scan_info = scan_dummy->GetDummyWorldInfo();
	const Vector3& dummy_view_center = scan_info.view_center;
	const Vector3& dummy_display_center = scan_info.display_center;
	const float dummy_scale_factor = scan_info.scale_factor;

	// main space
	// will be one frame slower...
	const auto& comp_camera_main = GetOwner().GetComponentManager().GetComponent<ComponentCamera>(m_comp_id_reference_camera_main);
	const Vector3 main_position = camera_main->GetTransform().GetPositionGlobal();
	const Vector3 main_target = comp_camera_main.GetTarget();
	const Vector3 main_up = comp_camera_main.GetUp();
	// XMMATRIX view_matrix_main = CameraMath::CalculateViewMatrix(main_position, main_target, main_up);

	// space conversion
	// naive
	Vector3 virtual_forward = (main_target - dummy_display_center) * dummy_scale_factor;
	virtual_forward.x *= -1.0f;
	virtual_forward.z *= -1.0f;
	virtual_forward.y *= -1.0f;
	const Vector3 virtual_target = dummy_view_center;// virtual_position + virtual_forward; // TODO: rotation
	const Vector3 virtual_position = virtual_target - virtual_forward;// (main_position - dummy_display_center) * dummy_scale_factor + dummy_view_center; // TODO: scale

	// XMMATRIX view_matrix_virtual = CameraMath::CalculateViewMatrix(virtual_position, virtual_target, main_up);
	// comp_camera.SetViewMatrix(view_matrix_virtual);
	m_transform.SetPosition(virtual_position); // updated to component
	comp_camera.SetTarget(virtual_target);
	comp_camera.SetUp(main_up);
}
