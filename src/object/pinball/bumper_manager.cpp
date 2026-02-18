#include "bumper_manager.h"
#include "object/game_object_list.h"
#include "object/pinball/bumper.h"

void BumperManager::Initialize()
{
}

void BumperManager::InitializeConfig(const FieldConfig& config)
{
	m_field_config = config;
	m_bumper_lists.resize(m_field_config.rooms.size());
	for (auto& list : m_bumper_lists)
	{
		list.clear();
	}
}

void BumperManager::InitializeRoom(int room_id, std::weak_ptr<Room> room)
{
	const auto& room_config = m_field_config.rooms.at(room_id);
	auto& bumper_list = m_bumper_lists.at(room_id);
	for (const auto& bumper_config : room_config.bumpers)
	{
		auto bumper = GetOwner().CreateGameObject<class Bumper>();
		bumper->InitializeConfig(bumper_config, room);
		bumper_list.push_back(bumper);
	}
}

void BumperManager::SetRoomTransIn(int room_id, float duration)
{
	auto& bumper_list = m_bumper_lists.at(room_id);
	for (auto& bumper : bumper_list)
	{
		bumper.lock()->EnterTransIn(duration);
	}
}

void BumperManager::SetRoomTransOut(int room_id, float duration)
{
	auto& bumper_list = m_bumper_lists.at(room_id);
	for (auto& bumper : bumper_list)
	{
		bumper.lock()->EnterTransOut(duration);
	}
}
