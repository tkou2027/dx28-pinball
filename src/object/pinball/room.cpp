#include "room.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "component/render/component_renderer_particle.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
// physics
#include "component/physics/component_collider.h"
// parts
#include "object/game_object_list.h"
#include "object/pinball/field.h"
#include "object/pinball/bumper_manager.h"
#include "object/pinball/player.h"
#include "object/pinball/camera_follow.h"
#include "object/pinball/room_floor.h"
#include "object/pinball/room_goal.h"
#include "object/pinball/enemy_center.h"
// utils
#include "game_util/move_controller.h"

//#include "object/effect/test_texture.h"

void Room::Initialize()
{
	//// render
	//m_components.Add<ComponentRendererMesh>(m_comp_id_render);
}

void Room::OnSceneInitialized()
{
	m_bumper_manager = GetOwner().FindGameObject<BumperManager>();
	assert(m_bumper_manager.lock());
	m_player = GetOwner().FindGameObject<Player>();
	assert(m_player.lock());
	m_camera = GetOwner().FindGameObject<CameraFollow>();
	assert(m_camera.lock());
}

void Room::InitializeConfig(const FieldConfig& field_config, int id, std::weak_ptr<Field> field)
{
	m_field_config = field_config;
	m_room_id = id;
	m_config = field_config.rooms[id];
	m_transform.SetPositionY(m_config.floor.height);
	m_field = field;

	InitializeFloor();
	InitializeGoals();
	InitializeEnemies();
	InitializeBackground();

	EnterRoomStateIdle();
}

void Room::Update()
{
	switch (m_state)
	{
	case RoomState::TRANS_IN_STAGE_1:
	{
		UpdateRoomStateTransInFirst();
		break;
	}
	case RoomState::TRANS_IN_STAGE_2:
	{
		UpdateRoomStateTransInSecond();
		break;
	}
	case RoomState::ACTIVE:
	{
		UpdateRoomStateActive();
		break;
	}
	case RoomState::TRANS_OUT_STAGE_1:
	{
		UpdateRoomStateTransOutFirst();
		break;
	}
	case RoomState::TRANS_OUT_STAGE_2:
	{
		UpdateRoomStateTransOutSecond();
		break;
	}
	}

	//auto& border_model = m_components.Get<ComponentRendererMesh>(m_comp_id_render).GetModel(border_index);
	//auto& uv_state = border_model.GetUVAnimationState();
	//uv_state.uv_scroll_offset = uv_state.uv_scroll_offset + Vector2{ 0.001f, 0.0f };
}

// states
void Room::EnterRoomStateIdle()
{
	m_state = RoomState::IDLE;
	// disable components
	//auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//comp_render.SetActive(false);

	m_room_floor.lock()->EnterIdle();
}

void Room::EnterRoomStateTransIn()
{
	m_state = RoomState::TRANS_IN_STAGE_1;
	m_state_countdown.Initialize(m_action_config.trans_in_out_duration_stage_1); // TODO config

	m_room_floor.lock()->EnterTransIn(m_action_config.trans_in_out_duration_stage_1);
	// TODO: EnterAnimation
	m_bumper_manager.lock()->InitializeRoom(m_room_id, std::dynamic_pointer_cast<Room>(shared_from_this()));

	auto player = m_player.lock();
	player->EnterControlled();
	if (IfFirstRoom())
	{
		player->GetTransform().SetPosition(m_config.player_trans_in_position);
	}
}

void Room::UpdateRoomStateTransInFirst()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateTransInSecond();
		return;
	}
	// TODO: update visuals
}

void Room::EnterRoomStateTransInSecond()
{
	m_state = RoomState::TRANS_IN_STAGE_2;
	m_state_countdown.Initialize(m_action_config.trans_in_out_duration_stage_2);

	// activate floor
	m_room_floor.lock()->EnterActive();
	// trans-in parts
	m_bumper_manager.lock()->SetRoomTransIn(m_room_id, m_action_config.trans_in_out_duration_stage_2);

	// activate components
	//auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//comp_render.SetActive(true);
	
	// activate parts
	for (auto& goal : m_room_goals)
	{
		goal.lock()->EnterActive();
	}
	for (auto& enemy_center : m_enemies_center)
	{
		enemy_center.lock()->EnterTransIn(m_action_config.trans_in_out_duration_stage_2);
	}
}

void Room::UpdateRoomStateTransInSecond()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateActive();
		return;
	}

	// update player movement to active position
	auto player = m_player.lock();
	MoveController::MoveTransform(
		player->GetTransform(),
		player->GetTransform().GetPosition(),//m_config.player_trans_in_position,
		m_config.player_active_position,
		1.0f - t
	);
	// update camera
	auto camera = m_camera.lock();
	camera->SetFollowCenteredTransfer(m_config.camera_center);
}

void Room::EnterRoomStateActive()
{
	m_state = RoomState::ACTIVE;
	auto player = m_player.lock();
	player->GetTransform().SetPosition(m_config.player_active_position);
	player->EnterMoveDefault();
	// TODO: EnterActiveAnimation done
	for (auto& enemy_center : m_enemies_center)
	{
		enemy_center.lock()->EnterAttack();
	}
}

void Room::UpdateRoomStateActive()
{
	if (CheckPlayerDead())
	{
		auto field = m_field.lock();
		field->StartFade();
		EnterRoomStateActive();
	}

	if (CheckClearState())
	{
		if (IfLastRoom())
		{
			auto field = m_field.lock();
			field->EnterExit();
			EnterRoomStateDone();
		}
		else
		{
			EnterRoomStateTransOut();
		}
	}
}

void Room::EnterRoomStateTransOut()
{
	// room clear hit stop
	m_state = RoomState::TRANS_OUT_STAGE_1;
	m_state_countdown.Initialize(m_action_config.trans_in_out_duration_stage_1);
	// room items trans out
	m_bumper_manager.lock()->SetRoomTransOut(m_room_id, m_action_config.trans_in_out_duration_stage_1);

	//auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//comp_render.SetActive(false);

	// next room enter trans in
	if (!IfLastRoom())
	{
		m_field.lock()->SetTransferActiveRoom(m_room_id + 1);
	}
}

void Room::UpdateRoomStateTransOutFirst()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateTransOutSecond();
		return;
	}
}

void Room::EnterRoomStateTransOutSecond()
{
	m_state = RoomState::TRANS_OUT_STAGE_2;
	m_state_countdown.Initialize(m_action_config.trans_in_out_duration_stage_2);
	// parts
	m_room_floor.lock()->EnterTransOut(m_action_config.trans_in_out_duration_stage_1);
}

void Room::UpdateRoomStateTransOutSecond()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateDone();
		return;
	}
}

void Room::EnterRoomStateDone()
{
	m_state = RoomState::DONE;
	// disable components
	// auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	// comp_render.SetActive(false);
	m_room_floor.lock()->EnterDone();
}

const Player& Room::GetPlayer() const
{
	return *m_player.lock();
}

Player& Room::GetPlayer()
{
	return *m_player.lock();
}

bool Room::CheckClearState() const
{
	for (auto& goal : m_room_goals)
	{
		if (!goal.lock()->IfDone())
		{
			return false;
		}
	}
	for (auto& enemy_center : m_enemies_center)
	{
		if (!enemy_center.lock()->IfDone())
		{
			return false;
		}
	}
	// TODO: other clear conditions...
	return true;
}

bool Room::CheckPlayerDead() const
{
	auto player = m_player.lock();
	auto& player_pos = player->GetTransform().GetPosition();
	bool fall = player_pos.y < (m_config.floor.height - 10.0f);
	return fall;
}

void Room::InitializeFloor()
{
	auto floor = GetOwner().CreateGameObject<RoomFloor>();
	floor->InitializeConfig(m_config, std::dynamic_pointer_cast<Room>(shared_from_this()));
	m_room_floor = floor;
}

void Room::InitializeGoals()
{
	for (const auto& goal_config : m_config.goals)
	{
		auto goal_obj = GetOwner().CreateGameObject<RoomGoal>();
		goal_obj->InitializeConfig(goal_config, std::dynamic_pointer_cast<Room>(shared_from_this()));
		m_room_goals.push_back(goal_obj);
	}
}

void Room::InitializeBackground()
{

}

void Room::InitializeEnemies()
{
	for (const auto& enemy_center_config : m_config.enemies_center)
	{
		auto enemy_center = GetOwner().CreateGameObject<EnemyCenter>();
		enemy_center->InitializeConfig(enemy_center_config, std::dynamic_pointer_cast<Room>(shared_from_this()));
		m_enemies_center.push_back(enemy_center);
	}
}

bool Room::IfFirstRoom() const
{
	return m_room_id == 0;
}

bool Room::IfLastRoom() const
{
	return m_room_id == m_field_config.rooms.size() - 1;
}
