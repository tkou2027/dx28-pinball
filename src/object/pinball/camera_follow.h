#pragma once
#include "object/game_object.h"
#include "render/config/camera_data.h"

class CameraFollow : public GameObject
{
public:
	enum class FollowMode
	{
		STATIC,
		FIXED, // follow target, keep fixed offset
		CENTERED, // follow target, rotate offset along target to center
		ROTATE // rotates towards center
	};
	struct CameraFollowConfig
	{
		FollowMode follow_mode{ FollowMode::FIXED };
		float smooth_factor{ 60.0f };
		// target
		float max_speed{ 120.0f };
		float min_speed{ 1.0f };
		float max_delay_distance{ 20.0f };
		// rotation
		float max_rotation_speed{ Math::TWO_PI };
	};
	struct CameraShakeConfig
	{
		Vector3 strength{ 0.0f, 0.0f, 0.0f };
		float duration{ 0.0f };
	};
	void Initialize() override;
	void Update() override;
	void SetTargetObject(std::weak_ptr<GameObject> target, const Vector3& target_position_offset);
	//void SetFollowFixed();
	void SetFollowFixed(const Vector3& offset);
	void SetFollowCentered(const Vector3& center, const Vector3& offset);
	void SetFollowCentered(const Vector3& center);
	void SetFollowCenteredTransfer(const Vector3& center);

	void SetShake(const CameraShakeConfig config);

	void GetDirectionXZNormalized(Vector3& out_forward, Vector3& out_right) const;

	// editor
	CameraShapeConfig& GetCameraShapeConfig() { return m_camera_shape_config; }
	void SetCameraShapeConfig(const CameraShapeConfig& config);
	Vector3& GetTargetPositionOffset() { return m_target_position_offset; }
	Vector3& GetFollowOffset() { return m_follow_offset_fixed; }
private:
	struct CameraShakeState
	{
		bool shaking{ false };
		float countdown{ 0.0f };
		Vector3 shake_offset{ 0.0f, 0.0f, 0.0f };
	};
	void UpdateImmediate();
	void UpdateFollow();
	void UpdateFollowTarget(bool immediate = false);
	void UpdateFollowOffset(bool immediate = false);
	void UpdateShake();

	ComponentId m_comp_id_camera{};
	// follow
	FollowMode m_follow_mode{ FollowMode::STATIC };
	CameraFollowConfig m_follow_config{};
	std::weak_ptr<GameObject> m_target_object{};
	Vector3 m_target_position_offset{ 0.0f, 0.0f, 0.0f };
	Vector3 m_follow_target{};
	Vector3 m_follow_offset_current{};

	Vector3 m_follow_offset_fixed{ 0.0f, 1.0f, -1.0f }; // fixed direction for fixed mode
	Vector3 m_rotation_center{ 0.0f, 0.0f, 0.0f }; // rotation center for cetered mode

	// shake
	CameraShakeConfig m_shake_config{};
	CameraShakeState m_shake_state{};

	CameraShapeConfig m_camera_shape_config{};
};