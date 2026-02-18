#pragma once

#include <vector>
#include <memory>

#include "object/game_object.h"
#include "game_util/field_config.h"

class BumperManager : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(const FieldConfig& config);
	void InitializeRoom(int room_id, std::weak_ptr<class Room> room);
	void SetRoomTransIn(int room_id, float duration);
	void SetRoomTransOut(int room_id, float duration);
	using BumperList = std::vector<std::weak_ptr<class Bumper> >;
private:
	std::vector<BumperList> m_bumper_lists; // per room

	FieldConfig m_field_config{};
};