#pragma once
#include "object/game_object.h"

class CameraEditor : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void SetFocus(const Vector3& target, const Vector3& offset);
	// camera movements
	void Zoom(float distance);
	void RotateXZ(float radius);
	struct CameraEditorConfig
	{
		float zoom_speed_min{ 10.0f };
		float zoom_speed_max{ 600.0f };
		float zoom_speed_factor{ 1.0f };
		float rotate_speed{ Math::PI / 4.0f };
	};
private:
	float ComputeZoomSpeed() const;
	ComponentId m_comp_id_camera{};
	CameraEditorConfig m_config{};
};