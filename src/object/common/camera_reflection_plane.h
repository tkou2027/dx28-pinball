#pragma once
#include "object/game_object.h"
#include "render/config/camera_data.h"
#include "config/camera_names.h"

class CameraReflectionPlane : public GameObject
{
public:
	void Initialize() override;
	void SetReference(
		const CameraPreset& camera_preset,
		std::weak_ptr<class GameObject> camera_reference,
		const Vector3& plane_pos,
		const Vector3& plane_normal
	);
	void Update() override;
private:
	void UpdateCameraTransform();
	ComponentId m_comp_id_camera{};
	CameraShapeConfig m_camera_shape_config{};

	std::weak_ptr<class GameObject> m_camera_reference;
	ComponentId m_comp_id_camera_reference{};
	// Vector3 m_plane_pos;
	// Vector3 m_plane_normal;
};