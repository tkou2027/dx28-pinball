#pragma once
#include "object/game_object.h"
#include "math/hex.h"
#include "util/countdown_timer.h"
#include "game_util/field_config.h"

class Player;
class Field;
class Room : public GameObject
{
public:
	void Initialize() override;
	void OnSceneInitialized() override;
	void InitializeConfig(const FieldConfig& field_config, int id,
		std::weak_ptr<Field> field);
	void Update() override;
	// for parts
	const RoomConfig& GetConfig() const { return m_config; }
	const class Player& GetPlayer() const;
	class Player& GetPlayer();
	// states
	void EnterRoomStateIdle();
	void EnterRoomStateTransIn();
private:
	void UpdateRoomStateTransInFirst();
	void EnterRoomStateTransInSecond();
	void UpdateRoomStateTransInSecond();
	void EnterRoomStateActive();
	void UpdateRoomStateActive();
	void EnterRoomStateTransOut();
	void UpdateRoomStateTransOutFirst();
	void EnterRoomStateTransOutSecond();
	void UpdateRoomStateTransOutSecond();
	void EnterRoomStateDone();

	bool CheckClearState() const;
	bool CheckPlayerDead() const;

private:
	enum class RoomState
	{
		IDLE,
		TRANS_IN_STAGE_1, // floor
		TRANS_IN_STAGE_2, /// player & item
		ACTIVE,
		TRANS_OUT_STAGE_1, // player move
		TRANS_OUT_STAGE_2, // fall
		DONE
	};
	// parts initialization
	void InitializeFloor();
	void InitializeGoals();
	void InitializeBackground();
	void InitializeEnemies();

	bool IfFirstRoom() const;
	bool IfLastRoom() const;

	// config
	FieldConfig m_field_config{};
	int m_room_id{};
	RoomConfig m_config{};
	struct RoomActionConfig
	{
		float trans_in_out_duration_stage_1{ 1.0f };
		float trans_in_out_duration_stage_2{ 2.0f };
	};
	RoomActionConfig m_action_config{};
	// state
	RoomState m_state{ RoomState::IDLE };
	CountdownTimer m_state_countdown{};
	// parts
	std::weak_ptr<class Field> m_field{};
	std::weak_ptr<class Player> m_player{};
	std::weak_ptr<class CameraFollow> m_camera{};
	std::weak_ptr<class BumperManager> m_bumper_manager{};
	std::weak_ptr<class RoomFloor> m_room_floor{};
	std::vector<std::weak_ptr<class RoomGoal> > m_room_goals{};
	std::vector<std::weak_ptr<class Bumper> > m_bumpers{};
	std::vector < std::weak_ptr<class EnemyCenter> > m_enemies_center{};
};