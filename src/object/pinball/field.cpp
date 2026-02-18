#include "field.h"

#include "component/render/component_renderer_mesh.h"

#include "object/game_object_list.h"

#include "global_context.h"
#include "config/preset_manager.h"
#include "render/render_system.h"
#include "render/render_resource.h"

#include "object/pinball/room.h"
#include "object/pinball/player.h"
#include "object/pinball/camera_follow.h"
#include "object/ui/fade.h"
#include "object/ui/button_enter.h"

void Field::Initialize()
{
	auto fade = GetOwner().CreateGameObject<Fade>();
	m_fade = fade;
}

void Field::Update()
{
	switch (m_state)
	{
		
	}
}

void Field::OnSceneInitialized()
{
	m_player = GetOwner().FindGameObject<Player>();
	assert(m_player.lock());
	m_camera = GetOwner().FindGameObject<CameraFollow>();
	assert(m_camera.lock());
}

void Field::InitializeConfig(const FieldConfig& config)
{
	m_config = config;
	// initialize rooms
	m_rooms.reserve(m_config.rooms.size());
	for (int i = 0; i < m_config.rooms.size(); i++)
	{
		auto room = GetOwner().CreateGameObject<Room>();
		room->InitializeConfig(config, i,
			std::dynamic_pointer_cast<Field>(shared_from_this()));
		m_rooms.push_back(room);
	}
	m_state = FieldState::ACTIVE;
}

void Field::SetInitiateActiveRoom(int room_id)
{
	// set player pos and camera
	// allow reset same room
	const auto& room_config = m_config.rooms.at(room_id);
	m_player.lock()->GetTransform().SetPosition(
		room_config.player_active_position);
	m_camera.lock()->SetFollowCentered(
		room_config.camera_center, { 0.0f, 40.0f, -50.0f });
	m_current_room_id = room_id;
	m_rooms.at(room_id).lock()->EnterRoomStateTransIn();
}

void Field::SetTransferActiveRoom(int room_id)
{
	if (m_current_room_id == room_id)
	{
		return;
	}
	if (m_current_room_id != -1)
	{
		// TODO previouse room trans out
	}
	m_current_room_id = room_id;
	m_rooms.at(room_id).lock()->EnterRoomStateTransIn();
}

void Field::EnterExit()
{
	m_state = FieldState::DONE; // TODO
}

bool Field::IfDone() const
{
	return m_state == FieldState::DONE;
}

void Field::StartFade()
{
	auto fade = m_fade.lock();
	fade->StartFadeOut();
}
