#pragma once
#include "object/game_object.h"
#include "render/config/camera_data.h"

class CameraMonitor : public GameObject
{
public:
	void Initialize() override;
	void SetTarget(const Vector3 target);
	CameraShapeConfig& GetCameraShapeConfig() { return m_camera_shape_config; }
	void SetCameraShapeConfig(const CameraShapeConfig& config);
private:
	ComponentId m_comp_id_camera{};
	CameraShapeConfig m_camera_shape_config{};
};