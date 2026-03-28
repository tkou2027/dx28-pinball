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

	AddUpdateLayer(UpdateLayer::HIT_STOP);
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
	auto& comp_camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	auto scan_dummy = m_scan_dummy.lock();
	auto camera_main = m_camera_main.lock();
	if (!scan_dummy || !camera_main)
	{
		comp_camera.SetActive(false);
		return;
	}
	comp_camera.SetActive(true);
	// dummy space
	auto scan_info = scan_dummy->GetDummyWorldInfo();
	const Vector3& dummy_view_center = scan_info.view_center;
	const Vector3& dummy_view_center_forward = scan_info.view_center_forward;
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
	Vector3 virtual_forward = (main_position - main_target).GetNormalized();
	float virtual_forward_xz_length = Vector2{virtual_forward.x, virtual_forward.z}.Length();
	Vector3 dummy_view_center_forward_xz = dummy_view_center_forward;
	dummy_view_center_forward_xz.y = 0.0f;
	dummy_view_center_forward_xz.Normalize();
	virtual_forward = Vector3{
		dummy_view_center_forward_xz.x * virtual_forward_xz_length,
		virtual_forward.y,
		dummy_view_center_forward_xz.z * virtual_forward_xz_length
	};
	// virtual_forward = virtual_forward * 0.1f;// dummy_scale_factor;
	//const Vector3 virtual_target = (main_target - main_position + dummy_view_center) * 0.1f;
	//const Vector3 virtual_position = dummy_view_center + virtual_forward; // TODO: rotation
	

	Vector3 virtual_from_center = (main_position - dummy_display_center) * 0.015f; // dummy_scale_factor;
	float virtual_from_center_xz_length = Vector2{ virtual_from_center.x, virtual_from_center.z }.Length();
	virtual_from_center = Vector3{
		dummy_view_center_forward_xz.x * virtual_from_center_xz_length,
		virtual_from_center.y,
		dummy_view_center_forward_xz.z * virtual_from_center_xz_length
	};


	const Vector3 virtual_position = dummy_view_center + virtual_from_center;
	const Vector3 virtual_target = virtual_position - virtual_forward;

	m_transform.SetPosition(virtual_position); // updated to component
	comp_camera.SetTarget(virtual_target);
	comp_camera.SetUp(main_up);
}
