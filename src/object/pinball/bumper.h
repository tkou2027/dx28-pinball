#pragma once
#include "object/game_object.h"
#include "game_util/field_config.h"
#include "util/countdown_timer.h"

class Room;
class Bumper : public GameObject
{
public:
	enum class BumperState
	{
		IDLE,
		TRANS_IN,
		ACTIVE,
		TRANS_OUT,
		DONE
	};
	enum class BumpedState
	{
		NONE,
		BUMPED,
		BUMPED_SUCCESS
	};
	void Initialize() override;
	void Update() override;
	void InitializeConfig(const BumperConfig& config, std::weak_ptr<Room> room);
	void OnPlayerCollides(bool success);
	const struct Collider& GetCollider() const;
	Vector3 GetSnapPosition(const Vector3& aim_pos) ;
	bool IfActive() const { return m_state == BumperState::ACTIVE; }

	void EnterIdle();
	void EnterTransIn(float duration);
	void EnterActive();
	void EnterTransOut(float duration);
	void EnterDone();
private:
	void InitializeVisual();
	void InitializeCollision();
	void ResetSubStates();
	void UpdateTransIn();
	void UpdateActive();
	void UpdateTransOut();
	void UpdateModelTransInOut();

	// parts
	ComponentId m_comp_id_render{};
	ComponentId m_comp_id_collider{};
	ComponentId m_comp_id_light{};
	int m_bumper_model_index{ -1 };
	int m_pointer_model_index{ -1 };

	// state
	BumperState m_state{ BumperState::IDLE };
	BumpedState m_bumped_state{ BumpedState::NONE };
	float m_trans_in_out_factor{ 1.0f };
	CountdownTimer m_state_countdown{};
	CountdownTimer m_bumped_timer{};

	std::weak_ptr<Room> m_room;
	BumperConfig m_config{};
};