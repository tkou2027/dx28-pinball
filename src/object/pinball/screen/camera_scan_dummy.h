#pragma once
#include "object/game_object.h"

#include "math/camera_math.h"

class CameraScanDummy : public GameObject
{
public:
	struct ScanInfo
	{
		Vector3 view_center{};
		Vector3 display_center{};
		Vector3 camera_position{};
		float scale_factor{ 0.05f };
		float rotation_y{ 0.0f };
	};
	void Initialize() override;
	void InitializeConfig();
	void Update() override;
	void Finalize() override {}
	ScanInfo GetDummyWorldInfo() const;
	void SetViewCenter(const Vector3& view_center, float rotation_y);
private:
	void UpdateFollow();
	// parts
	std::weak_ptr<class Projector> m_projector{};
	// world pos where dummy camera focus
	Vector3 m_view_center{};
	float m_rotation_y{ 0.0f }; // facing
	// world pos where vr screen are placed
	Vector3 m_display_center{};

	// TODO
	CameraShapeConfig m_projector_shape{};
};