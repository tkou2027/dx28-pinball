#pragma once
#include "object/game_object.h"
#include "game_util/enemy_config.h"
#include "util/countdown_timer.h"

class EnemyCenter : public GameObject
{
public:
	void Initialize() override;
	void OnSceneInitialized() override;
	void InitializeConfig(const EnemyCenterConfig& enemy_config, std::weak_ptr<class Room> room);
	void Update() override;
	bool IfDone() const { return m_state == EnemyState::DONE; }

	void EnterTransIn(float duration);
	void EnterAttack();
	void OnDamaged(float damage, const Vector3& position);
private:
	void InitializeCollision();
	void InitializeVisual();
	void InitializeStatus();

	// trans in
	void UpdateTransIn();
	// attack
	void UpdateAttack();
	void EnterAttackPrepare();
	void UpdateAttackPrepare();
	void EnterAttackFollow();
	void UpdateAttackFollow();
	void CheckAttackPhaseTransition();
	void EnterAttackExecute();
	void UpdateAttackExecute();
	void EnterAttackCooldown();
	void UpdateAttackCooldown();
	void ExitAttackCooldown();
	void UpdateNextAttack();
	// hurt
	void EnterHurt();
	void UpdateHurt();
	// follow
	void UpdateAimCameraFollow();
	void UpdateAimUIFollow();
	// aim utils
	void UpdateAimVisuals();
	// visual utils
	void AdjustShapeConfig();
	// bumpers
	void DestroyBumpers();
	// exit
	void EnterExit();
	void UpdateExit();

	ComponentId m_comp_id_collider{};
	// references
	std::weak_ptr<class Room> m_room;
	std::weak_ptr<class Player> m_player;
	std::vector<std::weak_ptr<class Bumper> > m_active_bumpers;
	// parts ====
	std::weak_ptr<class EnemyCenterVisual> m_visual;
	// screen texture generators
	std::weak_ptr<class ScreenStatusUI> m_status_ui;
	std::weak_ptr<class ScreenMainUI> m_main_ui;
	std::weak_ptr<class CameraScanDummy> m_camera_scan_dummy;
	std::weak_ptr<class CameraScan> m_camera_scan;

	enum class EnemyState
	{
		IDLE,
		TRANS_IN,
		// active states
		ATTACK,
		HURT,
		EXIT,
		DONE
	};
	enum class AttackState
	{
		IDLE,
		PREPARE,
		FOLLOW,
		EXECUTE,
		COOLDOWN,
		DONE
	};
	struct EnemyStatus
	{
		float health{};
		int health_index{ 0 };
		CountdownTimer state_countdown{};
		// attack
		int attack_phase_index{ -1 };
		int attack_index{ -1 };
		AttackState attack_state{ AttackState::IDLE };
		CountdownTimer attack_countdown{};
		// aim
		Vector3 aim_position{};
		float aim_player_rotation{};
		float aim_ui_rotation{ Math::PI };
		// lerp
		float aim_radius{};
		float aim_radius_start{};
		float aim_radius_end{};
		float aim_thickness{};
		float aim_thickness_start{};
		float aim_thickness_end{};
	};
	EnemyState m_state{};
	EnemyStatus m_status{};

	EnemyCenterConfig m_config{};
};