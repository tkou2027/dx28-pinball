#include "camera_reflection_plane.h"
#include "component/render/component_camera.h"
#include "object/game_object_list.h"

void CameraReflectionPlane::Initialize()
{
	auto& camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	camera.SetActive(false);
}

void CameraReflectionPlane::SetReference(
	const CameraPreset& camera_preset, std::weak_ptr<class GameObject> camera_reference_weak, const Vector3& plane_pos, const Vector3& plane_normal)
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetActive(true);

	// reference camera
	m_camera_reference = camera_reference_weak;
	auto camera_reference = m_camera_reference.lock();
	assert(camera_reference);
	m_comp_id_camera_reference =
		camera_reference->GetComponents().TryGetComponent<ComponentCamera>();
	const auto& comp_camera_reference = GetOwner().GetComponentManager().GetComponent<ComponentCamera>(
		m_comp_id_camera_reference);

	CameraUsageConfig config{};
	config.type = CameraType::REFLECTION_PLANE;
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;
	config.invert_culling = true;
	camera.InitializeCamera(config, comp_camera_reference.GetShapeConfig());
	// reflection
	CameraPlaneReflectionConfig reflection_plane_config{};
	reflection_plane_config.plane_position = plane_pos;
	reflection_plane_config.plane_normal = plane_normal.GetNormalized();
	camera.SetReflectionPlaneConfig(reflection_plane_config);

	//m_plane_pos = plane_pos;
	//m_plane_normal = plane_normal.GetNormalized();
	UpdateCameraTransform();
}

void CameraReflectionPlane::Update()
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	auto camera_reference = m_camera_reference.lock();
	if (!camera_reference)
	{
		camera.SetActive(false);
		return;
	}
	camera.SetActive(true);
	UpdateCameraTransform();
}

void CameraReflectionPlane::UpdateCameraTransform()
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	auto camera_reference = m_camera_reference.lock();
	const auto& comp_camera_reference = GetOwner().GetComponentManager().GetComponent<ComponentCamera>(m_comp_id_camera_reference);
	// shape
	camera.SetShapeConfig(comp_camera_reference.GetShapeConfig());
	// transform
	Vector3 main_pos = comp_camera_reference.GetPosition();
	Vector3 main_target = comp_camera_reference.GetTarget();
	Vector3 main_up = comp_camera_reference.GetUp();

	m_transform.SetPosition(main_pos);
	camera.SetTarget(main_target);
	camera.SetUp(main_up);
}
