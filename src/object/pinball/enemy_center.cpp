#include "enemy_center.h"
#include "component/physics/component_collider.h"
#include "object/game_object_list.h"
#include "object/pinball/enemy_center_visual.h"
#include "object/pinball/room.h"
#include "object/pinball/bumper.h"
#include "object/pinball/player.h"
#include "object/pinball/screen/screen_status_ui.h"
#include "object/pinball/screen/screen_main_ui.h"
#include "object/pinball/screen/camera_scan_dummy.h"
#include "object/pinball/screen/camera_scan.h"
#include "object/pinball/camera_follow.h"
#include "util/tween.h"

void EnemyCenter::Initialize()
{
	m_components.Add<ComponentCollider>(m_comp_id_collider);
}

void EnemyCenter::OnSceneInitialized()
{
	auto status_ui = GetOwner().FindGameObject<ScreenStatusUI>();
	assert(status_ui);
	m_status_ui = status_ui;

	auto player = GetOwner().FindGameObject<Player>();
	assert(player);
	m_player = player;
}

void EnemyCenter::InitializeConfig(const EnemyCenterConfig& enemy_config, std::weak_ptr<Room> room)
{
	m_config = enemy_config;
	m_room = room;
	InitializeCollision();
	InitializeVisual();
	InitializeStatus();
}

void EnemyCenter::Update()
{
	switch (m_state)
	{
	case EnemyState::IDLE:
	{
		break;
	}
	case EnemyState::TRANS_IN:
	{
		UpdateTransIn();
		break;
	}
	case EnemyState::ATTACK:
	{
		UpdateAttack();
		break;
	}
	case EnemyState::HURT:
	{
		UpdateHurt();
		break;
	}
	case EnemyState::EXIT:
	{
		UpdateExit();
		break;
	}
	}
}

void EnemyCenter::EnterTransIn(float duration)
{
	m_state = EnemyState::TRANS_IN;
	// timer
	m_status.state_countdown.Initialize(duration);
}

void EnemyCenter::UpdateTransIn()
{
	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		// EnterActive();
		return;
	}
	// update visuals
}

void EnemyCenter::EnterAttack()
{
	auto& attack_phases = m_config.attack_phases;
	m_state = EnemyState::ATTACK;
	if (attack_phases.size() == 0 || attack_phases[0].attacks.size() == 0)
	{
		m_status.attack_state = AttackState::IDLE;
		return;
	}
	m_status.attack_phase_index = 0;
	m_status.attack_index = 0;
	EnterAttackPrepare();
}

void EnemyCenter::OnDamaged(float damage, const Vector3& position)
{
	if (damage <= 0.0f)
	{
		return;
	}
	if (m_state != EnemyState::ATTACK)
	{
		return;
	}
	// enter damaged state
	m_status.health -= damage;
	m_status_ui.lock()->UpdateValue(m_status.health);

	// hurt position
	Vector3 hurt_direction = position - m_config.shape.position;
	hurt_direction.y = 0.0f;
	if (hurt_direction.LengthSquared() > Math::EPSILON)
	{
		hurt_direction.Normalize();
		m_status.aim_position = m_config.shape.position + hurt_direction * m_config.shape.side_radius;
		// radius from 0
		m_status.aim_radius_start = 0.0f;
		m_status.aim_radius_end = m_config.aim_ui.radius_max;
		m_status.aim_radius = m_status.aim_radius_start;
		// thickness from 0
		m_status.aim_thickness_start = 0.0f;
		m_status.aim_thickness_end = m_config.aim_ui.thickness_max;
		m_status.aim_thickness = m_status.aim_thickness_start;
		UpdateAimCameraFollow();
		UpdateAimUIFollow();
		UpdateAimVisuals();
	}

	EnterHurt();
}

void EnemyCenter::InitializeCollision()
{
	const auto& shape_config = m_config.shape;
	// collision
	PolygonCylinder cylinder{};
	cylinder.height = shape_config.side_height;
	cylinder.radius = shape_config.side_radius;
	cylinder.sides = shape_config.sides;
	TransformNode3D shape_transform{};
	shape_transform.SetParent(&m_transform);
	shape_transform.SetPosition(shape_config.position);
	shape_transform.SetRotationYOnly(Math::PI * 0.25f);
	Collider collider{ &m_transform, ColliderLayer::Type::ENEMY };
	collider.AddShape(ShapeCollider{ cylinder }, shape_transform);
	collider.bounce = 0.9f;
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.AddCollider(collider);
}

void EnemyCenter::InitializeVisual()
{
	auto visual = GetOwner().CreateGameObject<EnemyCenterVisual>();
	visual->InitializeConfig(m_config.shape);
	visual->GetTransform().SetParent(&m_transform);
	m_visual = visual;
}

void EnemyCenter::InitializeStatus()
{
	// initial health
	m_status.health = m_config.health;
	// initial focus
	auto room = m_room.lock();
	const auto& room_config = room->GetConfig();
	const Vector3 player_initial_position = room_config.player_active_position;
	m_status.aim_position = player_initial_position + m_config.aim_offset;

	// status parts
	auto status_ui = GetOwner().CreateGameObject<ScreenStatusUI>();
	status_ui->InitializeConfig(m_config.health);
	m_status_ui = status_ui;
	auto main_ui = GetOwner().CreateGameObject<ScreenMainUI>();
	main_ui->InitializeConfig(m_config.health);
	m_main_ui = main_ui;
	auto camera_scan_dummy = GetOwner().CreateGameObject<CameraScanDummy>();
	auto camera_scan = GetOwner().CreateGameObject<CameraScan>();
	camera_scan_dummy->InitializeConfig(); // TODO
	camera_scan->SetCameraMain(GetOwner().FindGameObject<CameraFollow>());
	camera_scan->SetReference(camera_scan_dummy);
	m_camera_scan_dummy = camera_scan_dummy;
	UpdateAimVisuals();
}

void EnemyCenter::UpdateAttack()
{
	switch (m_status.attack_state)
	{
	case AttackState::IDLE:
	{
		break;
	}
	case AttackState::PREPARE:
	{
		UpdateAttackPrepare();
		break;
	}
	case AttackState::FOLLOW:
	{
		UpdateAttackFollow();
		break;
	}
	case AttackState::EXECUTE:
	{
		UpdateAttackExecute();
		break;
	}
	case AttackState::COOLDOWN:
	{
		UpdateAttackCooldown();
		break;
	}
	}
	// TODO
	CheckAttackPhaseTransition();
}

void EnemyCenter::EnterAttackPrepare()
{
	m_state = EnemyState::ATTACK;
	m_status.attack_state = AttackState::PREPARE;
	auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
	auto& attack_config = phase_config.attacks[m_status.attack_index];
	// initialize attack visuals
	// initialize timers
	m_status.attack_countdown.Initialize(attack_config.prepare_duration);
}

void EnemyCenter::UpdateAttackPrepare()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterAttackFollow();
		return;
	}
	// update aim position
	UpdateAimCameraFollow();

	// update aim visuals
	UpdateAimVisuals();
}

void EnemyCenter::EnterAttackFollow()
{
	m_status.attack_state = AttackState::FOLLOW;
	auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
	auto& attack_config = phase_config.attacks[m_status.attack_index];
	// initialize attack visuals

	// initialize timers
	m_status.attack_countdown.Initialize(attack_config.follow_duration);
}

void EnemyCenter::UpdateAttackFollow()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterAttackExecute();
		return;
	}
	// update aim position
	UpdateAimCameraFollow();
	// update aim visuals
	UpdateAimVisuals();
}

void EnemyCenter::CheckAttackPhaseTransition()
{

}

void EnemyCenter::EnterAttackExecute()
{
	m_status.attack_state = AttackState::EXECUTE;
	auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
	auto& attack_config = phase_config.attacks[m_status.attack_index];
	// timer
	m_status.attack_countdown.Initialize(attack_config.execute_duration);
	// visuals

}

void EnemyCenter::UpdateAttackExecute()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterAttackCooldown();
		return;
	}
}

void EnemyCenter::EnterAttackCooldown()
{
	m_status.attack_state = AttackState::COOLDOWN;
	auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
	auto& attack_config = phase_config.attacks[m_status.attack_index];
	// timer
	m_status.attack_countdown.Initialize(attack_config.cooldown_duration);
	// generate bumpers
	for (const auto& bumper_config : attack_config.bumpers)
	{
		auto bumper = GetOwner().CreateGameObject<Bumper>();
		bumper->InitializeConfig(bumper_config, m_room);
		bumper->EnterActive();
		m_active_bumpers.push_back(bumper);
	}
}

void EnemyCenter::UpdateAttackCooldown()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		ExitAttackCooldown();
		UpdateNextAttack();
		EnterAttackPrepare();
		return;
	}
}

void EnemyCenter::ExitAttackCooldown()
{
	DestroyBumpers();
}

void EnemyCenter::UpdateNextAttack()
{
	auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
	m_status.attack_index = (m_status.attack_index + 1) % phase_config.attacks.size();
	//if (m_status.attack_index >= static_cast<int>(phase_config.attacks.size()))
	//{
	//	// next phase
	//	m_status.attack_phase_index++;
	//	m_status.attack_index = 0;
	//	if (m_status.attack_phase_index >= static_cast<int>(m_config.attack_phases.size()))
	//	{
	//		// loop to first phase
	//		m_status.attack_phase_index = 0;
	//	}
	//}
}

void EnemyCenter::EnterHurt()
{
	m_state = EnemyState::HURT;
	m_status.state_countdown.Initialize(m_config.damage_delay);
}

void EnemyCenter::UpdateHurt()
{
	float t = m_status.state_countdown.Update(GetDeltaTime());
	// clear all bumpers
	if (t <= 0.0f)
	{
		// return to attack state
		m_status.aim_radius_start = 0.0f;
		m_status.aim_radius_end = 0.0f;
		m_status.aim_thickness_start = 0.0f;
		m_status.aim_thickness_end = 0.0f;
		m_status.aim_radius = 0.0f;
		m_status.aim_thickness = 0.0f;
		UpdateAimVisuals();
		DestroyBumpers();
		if (m_status.health <= 0)
		{
			// handle death
			EnterExit();
		}
		else
		{
			EnterAttackPrepare();
		}
		return;
	}
	if (t < 0.5)
	{
		t = 1.0f - t;
	}
	float t_lerp = Tween::EaseFunc(Tween::TweenFunction::EASE_OUT_QUAD, (1.0f - t) * 2.0f);
	m_status.aim_radius = Math::Lerp(m_status.aim_radius_start, m_status.aim_radius_end, t_lerp);
	m_status.aim_thickness = Math::Lerp(m_status.aim_thickness_start, m_status.aim_thickness_end, t_lerp);
	UpdateAimCameraFollow();
	UpdateAimVisuals();
}

void EnemyCenter::UpdateAimCameraFollow()
{
	auto player = m_player.lock();
	m_status.aim_position = player->GetTransform().GetPositionGlobal()
		+ m_config.aim_offset; // player offset
	m_status.aim_player_rotation = player->GetTransform().GetRotationYGlobal();
}

void EnemyCenter::UpdateAimUIFollow()
{
	Vector3 to_aim = m_status.aim_position - m_config.shape.position;
	m_status.aim_ui_rotation = atan2f(to_aim.x, to_aim.z);
}

void EnemyCenter::UpdateAimVisuals()
{
	// update screen ui
	// calculate position
	float rotation_ratio = (m_status.aim_ui_rotation + Math::PI * 0.25f) / (2.0f * Math::PI); // screen is rotated
	// update aim to screen ui
	auto main_ui = m_main_ui.lock();
	ScreenMainUI::AimInfo aim_info{};
	aim_info.rotation_ratio = rotation_ratio;
	aim_info.height_ratio = 0.5f;
	aim_info.radius = m_status.aim_radius;
	aim_info.thickness = m_status.aim_thickness;
	// TODO: radius and thickness
	main_ui->SetAimInfo(aim_info);

	// update focus
	auto camera_scan_dummy = m_camera_scan_dummy.lock();
	camera_scan_dummy->SetViewCenter(m_status.aim_position, m_status.aim_player_rotation);
}

void EnemyCenter::DestroyBumpers()
{
	for (auto& bumper_weak : m_active_bumpers)
	{
		if (auto bumper = bumper_weak.lock())
		{
			bumper->EnterDone();
		}
	}
	m_active_bumpers.clear();
}

void EnemyCenter::EnterExit()
{
	m_state = EnemyState::EXIT;
	// timer
	m_status.state_countdown.Initialize(m_config.exit_duration);
	// visuals
}

void EnemyCenter::UpdateExit()
{
	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		m_state = EnemyState::DONE;
	}
}
