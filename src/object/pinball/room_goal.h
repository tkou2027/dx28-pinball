#pragma once
#include "object/game_object.h"
#include "object/pinball/game_util/goal_config.h"
#include "util/countdown_timer.h"

class Player;
class Room;
class RoomGoal : public GameObject
{
public:
	enum class GoalState
	{
		IDLE,
		TRANS_IN,
		ACTIVE,
		EXIT,
		DONE
	};
	void Initialize() override;
	void Update() override;
	void InitializeConfig(const GoalConfig& config, std::weak_ptr<Room> room);
	void OnPlayerEnter(Player& player);
	void OnPlayerEnterFailed(const Player& player);
	bool IfDone() const;
private:
	void InitializeVisuals();
	void InitializeCollision();
	// states
	void EnterIdle();
public:
	void EnterActive();
private:
	void EnterExit();
	void EnterDone();
	void CheckUpdatePlayerSpeed();
	void UpdateVisual();
	void UpdatePlayer();

	// parts
	ComponentId m_comp_id_render{};
	ComponentId m_comp_id_render_particle{};
	ComponentId m_comp_id_render_particle_border{};
	ComponentId m_comp_id_collider{};
	std::weak_ptr<Room> m_room;

	// config ==== 
	GoalConfig m_config{};
	struct GoalActionConfig
	{
		float player_exit_duration{ 2.0f };
	};
	GoalActionConfig m_action_config{};

	// states
	GoalState m_state{ GoalState::IDLE };
	CountdownTimer m_state_countdown{};
	struct GoalVisualState
	{
		int level_triggered{ 0 };
		int level_target{ 0 };
	};
	GoalVisualState m_visual_state{};

};