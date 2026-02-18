#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"

class Player : public GameObject
{
	friend class PlayerAim;
public:
	void Initialize() override;
	void OnSceneInitialized() override;
	void Update() override;
	void OnCollision(const struct CollisionInfo& collision) override;
	void OnTrigger(const struct CollisionInfo& collision) override;
	// communicate with field items
	float GetSpeedLevel() const;
	// states public
	void EnterMoveDefault();
	void EnterControlled();
private:
	// states
	void UpdateMove();
	void UpdateSpeedLevelAndGravity();
	void UpdateTarget();
	// orbit state
	void EnterOrbit();
	void UpdateOrbit();
	// shoot state
	void EnterDash();
	void UpdateDash();
	// bounce state
	bool CheckEnterBounce();
	//void EnterBounce();
	//void UpdateBounce();

	void HitEffect();

	// input 
	Vector3 GetMoveInput() const;
	// collision
	void OnCollisionBumper(const CollisionInfo& collision);
	void OnCollisionGoal(const CollisionInfo& collison); // goal failed
	void OnTriggerGoal(const CollisionInfo& collision); // goal success
	void OnCollisionEnemy(const CollisionInfo& collision);
	bool IfCollisionSpecial() const;
	// move utils
	void RotateToMoveDirection(const Vector3& move_dir);
	// speed level
	int GetSpeedLevelFromDistance(float distance) const;
	void TryUpdateSpeedLevel(int level);

	// components ========
	ComponentId m_comp_id_render;
	ComponentId m_comp_id_collider;
	// parts ========
	std::weak_ptr<PlayerAim> m_player_aim{};
	std::weak_ptr<class CameraFollow> m_camera{};
	std::weak_ptr<class HitStopUpdater> m_hit_stop_updater{};

	// move config ========
	struct MoveConfig
	{
		// default moving
		float max_speed_default{ 0.5f };
		float max_acc_default{ 0.02f };
		// dash and bump
		float dash_duration{ 1.0f / 6.0f };
		// dash speed
		int speed_level_max{ 5 };
		float distance_per_speed_level{ 8.0f };
		float speed_level_drop_interval{ 0.5f };
	};
	MoveConfig m_move_config{};

	// states ========
	enum class MoveState
	{
		DEFAULT,
		ORBIT,
		DASH,
		// BOUNCE,
		CONTROLLED
	};
	MoveState m_state{ MoveState::DEFAULT };
	// timer
	CountdownTimer m_state_countdown{};
	// orbit info
	struct BumpRecord
	{
		Vector3 position{};
		int collider_id{ -1 };
	};
	std::vector<BumpRecord> m_bump_records{};
	// move state
	CountdownTimer m_speed_level_timer{};
	Vector3 m_desired_velocity{};
	int m_speed_level{ 0 };

	// dash state
	Vector3 m_dash_init_pos;
	Vector3 m_dash_target_pos;
	float m_dash_countdown{ 0.0f };
	
	// goal state
	Vector3 m_goal_target_pos;
};