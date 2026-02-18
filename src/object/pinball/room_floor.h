#pragma once
#include "object/game_object.h"
#include "game_util/field_config.h"
#include "util/countdown_timer.h"
#include "math/hex.h"

class Room;
class RoomFloor : public GameObject
{
public:
	enum class FloorState
	{
		IDLE,
		TRANS_IN,
		ACTIVE,
		TRANS_OUT,
		DONE
	};
	void Initialize() override;
	void InitializeConfig(const RoomConfig& config, std::weak_ptr<Room> room);
	void Update() override;
	// states (controlled by room)
	void EnterIdle();
	void EnterTransIn(float duration);
	void EnterActive();
	void EnterTransOut(float duration);
	void EnterDone();
private:
	void InitializeVisuals();
	void InitializeCollision();
	void UpdateTransIn();
	void UpdateTransOut();
	bool UpdateBlockTransInOut(float y_start, float y_end, float t);

	// parts
	ComponentId m_comp_id_collider{};
	std::weak_ptr<Room> m_room;
	std::weak_ptr<class RoomFloorVisual> m_visual;
	// config
	RoomConfig m_config{};
	struct RoomFloorActionConfig
	{
		float trans_in_height_offset{ 30.0f };
		float trans_out_height_offset{ 30.0f };
	};
	// state
	FloorState m_state{ FloorState::IDLE };
	CountdownTimer m_state_countdown{};
	Hex m_hex{};
};