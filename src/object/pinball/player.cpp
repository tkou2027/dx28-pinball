#include "player.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "component/physics/component_collider.h"
// physics
#include "physics/physics_scene.h"
// other objects
#include "object/game_object_list.h"
#include "object/common/hit_stop_updater.h"
#include "object/pinball/camera_follow.h"
#include "object/pinball/bumper.h"
#include "object/pinball/player_aim.h"
#include "object/pinball/room_goal.h"
#include "object/pinball/enemy_center.h"

#include "global_context.h"
#include "platform/controller.h"
#include "platform/keyboard.h"
#include "platform/sound.h"
#include "config/audio_manager.h"

#include "util/debug_ostream.h"

void Player::Initialize()
{
	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render);
	auto& comp_collider = m_components.Add<ComponentCollider>(m_comp_id_collider);
	// character model
	{
		const auto& model_desc = GetPresetManager().GetModelDesc("model/character");
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.metallic = 1.0f;
		material_default.roughness = 0.1f;
		material.SetTechnique(material_default, CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::MONITOR);
		material.SetTechnique(TechniqueDescDeferredCel{}, CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::MONITOR);
		material.SetTechnique(TechniqueDescForwardSilhouette{});
		Model model{ model_desc, material, &m_transform };
		//model.GetTransform().SetScale(0.1f);
		comp_render_mesh.AddModel(model);
	}
	// collision
	{
		Sphere sphere{};
		sphere.radius = 1.5f;
		TransformNode3D shape_transform{};
		shape_transform.SetParent(&m_transform);
		shape_transform.SetPositionY(sphere.radius);
		Collider player_collider{ &m_transform, ColliderLayer::Type::PLAYER };
		player_collider.SetIsActor(true);
		player_collider.AddShape(ShapeCollider{ sphere }, shape_transform);
		comp_collider.AddCollider(player_collider);
	}

	const auto& player_aim = GetOwner().CreateGameObject<PlayerAim>();
	player_aim->SetPlayer(std::dynamic_pointer_cast<Player>(shared_from_this()));
	m_player_aim = player_aim;
}

void Player::OnSceneInitialized()
{
	m_camera = GetOwner().FindGameObject<CameraFollow>();
	assert(m_camera.lock());
	m_hit_stop_updater = GetOwner().FindGameObject<HitStopUpdater>();
	assert(m_hit_stop_updater.lock());
}

void Player::Update()
{
	// move state
	switch (m_state)
	{
	case MoveState::DEFAULT:
	{
		UpdateMove();
		UpdateTarget();
		break;
	}
	case MoveState::ORBIT:
	{
		UpdateMove();
		UpdateOrbit();
		break;
	}
	case MoveState::DASH:
	{
		UpdateDash();
		break;
	}
	case MoveState::CONTROLLED:
	{
		// position updated externally
		break;
	}
	}
	// common updates
	UpdateSpeedLevelAndGravity();
}

void Player::UpdateSpeedLevelAndGravity()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	auto& collider = comp_collider.GetCollider(0);
	// move state
	switch (m_state)
	{
	case MoveState::DEFAULT:
	{
		float t = m_speed_level_timer.Update(GetDeltaTime());
		if (t <= 0.0f)
		{
			TryUpdateSpeedLevel(m_speed_level - 1);
			m_speed_level_timer.Initialize(m_move_config.speed_level_drop_interval);
		}
		collider.gravity_scale = m_speed_level > 0 ? 0.0f : 1.0f;
		break;
	}
	case MoveState::ORBIT:
	{
		collider.gravity_scale = 1.0f;
		break;
	}
	case MoveState::DASH:
	case MoveState::CONTROLLED:
	{
		collider.gravity_scale = 0.0f;
		break;
	}
	}
}

void Player::OnCollision(const CollisionInfo& collision)
{
	const Collider* other = collision.other;
	switch (other->GetLayer())
	{
	case ColliderLayer::Type::BUMPER:
	{
		OnCollisionBumper(collision);
		break;
	}
	case ColliderLayer::Type::GOAL:
	{
		OnCollisionGoal(collision);
		break;
	}
	case ColliderLayer::Type::ENEMY:
	{
		OnCollisionEnemy(collision);
		break;
	}
	}
}

void Player::OnTrigger(const CollisionInfo& collision)
{
	const Collider* other = collision.other;
	switch (other->GetLayer())
	{
	case ColliderLayer::Type::GOAL:
	{
		OnTriggerGoal(collision);
		break;
	}
	}
}

void Player::OnCollisionBumper(const CollisionInfo& collision)
{
	switch (m_state)
	{
	case MoveState::DASH:
	//case MoveState::BOUNCE:
	{
		HitEffect();
		auto other_obj = collision.other->GetOwner();
		auto bumper = std::dynamic_pointer_cast<Bumper>(other_obj.lock());
		bumper->OnPlayerCollides(true);
		//if (CheckEnterBounce())
		//{
		//	// hal::dout << "Player Enter Bounce" << std::endl;
		//	EnterBounce();
		//}
		//else
		//{
		//	// hal::dout << "Player Exit Bounce" << std::endl;
		//	EnterMoveDefault();
		//}
		TryUpdateSpeedLevel(m_speed_level + 1);
		EnterMoveDefault();
		break;
	}
	}
	// hal::dout << "Player collided with bumper" << std::endl;
}

void Player::OnCollisionGoal(const CollisionInfo& collision)
{

	auto other_obj = collision.other->GetOwner();
	// TODO: other type ?
	auto goal = std::dynamic_pointer_cast<RoomGoal>(other_obj.lock());
	goal->OnPlayerEnterFailed(*this);
	if (IfCollisionSpecial())
	{
		HitEffect();
		EnterMoveDefault();
	}
	// hal::dout << "Player collided with goal" << std::endl;
}

void Player::OnTriggerGoal(const CollisionInfo& collision)
{
	auto other_obj = collision.other->GetOwner();
	// TODO: other type ?
	auto goal = std::dynamic_pointer_cast<RoomGoal>(other_obj.lock());
	goal->OnPlayerEnter(*this);
	if (IfCollisionSpecial())
	{
		HitEffect();
		EnterMoveDefault();
	}
	// hal::dout << "Player collided with goal" << std::endl;
}

void Player::OnCollisionEnemy(const CollisionInfo& collision)
{
	auto other_obj = collision.other->GetOwner();
	// TODO: other type ?
	auto enemy_center = std::dynamic_pointer_cast<EnemyCenter>(other_obj.lock());
	if (!enemy_center)
	{
		return;
	}
	float damage = m_speed_level * 10.0f;
	enemy_center->OnDamaged(damage, collision.hit_info.hit_position);
	if(IfCollisionSpecial())
	{
		HitEffect();
		EnterMoveDefault();
	}
}

bool Player::IfCollisionSpecial() const
{
	return m_state == MoveState::DASH ||
		(m_state == MoveState::DEFAULT && m_speed_level > 0);
}

float Player::GetSpeedLevel() const
{
	return m_speed_level;
}

void Player::EnterMoveDefault()
{
	m_state = MoveState::DEFAULT;
	// TryUpdateSpeedLevel(0);
	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	// collider.velocity = Vector3{};
	collider.SetActive(true);
	collider.gravity_scale = 1.0f;
}

void Player::EnterControlled()
{
	// TODO: check if already controlled?
	m_state = MoveState::CONTROLLED;
	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	collider.velocity = Vector3{};
	collider.SetActive(false);
	// TODO: reset states?
	// position will be updated externally
}

Vector3 Player::GetMoveInput() const
{
	auto& controller = g_global_context.m_controller;
	Vector2 input{ controller->GetLeftX(), controller->GetLeftY() };
	input.ClampMagnitude(1.0f);

	// keyboard
	{
		// input = { 0.0f, 0.0f };
		if (g_global_context.m_keyboard->IsKeyDown(KK_LEFT))
		{
			input.x -= 1.0f;
		}
		if (g_global_context.m_keyboard->IsKeyDown(KK_RIGHT))
		{
			input.x += 1.0f;
		}
		if (g_global_context.m_keyboard->IsKeyDown(KK_UP))
		{
			input.y += 1.0f;
		}
		if (g_global_context.m_keyboard->IsKeyDown(KK_DOWN))
		{
			input.y -= 1.0f;
		}
	}

	auto camera = m_camera.lock();
	Vector3 right{};
	Vector3 forward{};
	camera->GetDirectionXZNormalized(forward, right);
	Vector3 movement = right * input.x + forward * input.y;
	return movement;
}

void Player::UpdateMove()
{
	const Vector3 movement_input = GetMoveInput();
	m_desired_velocity = movement_input * m_move_config.max_speed_default;

	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	auto velocity = collider.velocity;

	float acc = m_move_config.max_acc_default;
	acc = acc * (1.0f - m_speed_level / static_cast<float>(m_move_config.speed_level_max)); // TODO: balance
	velocity.x = Math::MoveTowards(velocity.x, m_desired_velocity.x, acc);
	velocity.z = Math::MoveTowards(velocity.z, m_desired_velocity.z, acc);
	collider.velocity = velocity;
	//movement = movement * 0.01f;

	//if (controller->IsPressed(Controller::Button::B))
	//{
	//	movement.y = 5.0f; // jump
	//}
	if (m_speed_level == 0)
	{
		RotateToMoveDirection(movement_input);
	}
}

void Player::UpdateTarget()
{
	bool hit = m_player_aim.lock()->UpdateTarget();
	if (!hit)
	{
		return;
	}
	// if trigger lock-on
	if (g_global_context.m_controller->IsPressed(Controller::Button::A)
		|| g_global_context.m_keyboard->IsKeyDown(KK_SPACE))
	{
		EnterOrbit();
	}
}

void Player::EnterOrbit()
{
	//auto target = m_target_bumper.lock();
	//assert(target);
	m_state = MoveState::ORBIT;
	m_player_aim.lock()->UpdateOrbit();
}

void Player::UpdateOrbit()
{
	bool hit = m_player_aim.lock()->UpdateOrbit();
	if (!hit)
	{
		EnterMoveDefault();
		return;
	}

	// Check if shoot
	// pressed
	if (g_global_context.m_controller->IsDown(Controller::Button::A)
		|| g_global_context.m_keyboard->IsKeyDown(KK_SPACE))
	{
		return;
	}
	// on release
	// aim_line.SetActive(false);
	EnterDash();
}

void Player::EnterDash()
{
	auto& target_bumper = m_player_aim.lock()->GetTargetBumper();
	const Vector3& start_pos = m_transform.GetPosition();

	m_bump_records.clear();
	m_bump_records.push_back({
		start_pos, -1
	});
	m_dash_init_pos = m_transform.GetPosition();
	const Vector3 target_pos = target_bumper.GetSnapPosition(m_dash_init_pos);
	Vector3 to_target_dir = target_pos - m_dash_init_pos;
	to_target_dir.y = 0.0f;
	to_target_dir.Normalize();
	m_dash_target_pos = target_pos + to_target_dir * 10.0f; // overshoot

	// m_dash_target_pos = target->GetTransform().GetPositionGlobal();
	// m_dash_target_pos = m_dash_target_pos * 1.1f; // TODO: overshoot
	m_dash_target_pos.y = m_dash_init_pos.y; // TODO

	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	collider.velocity = Vector3{};

	m_state = MoveState::DASH;
	m_state_countdown.Initialize(m_move_config.dash_duration);
	m_player_aim.lock()->EnterHidden();
}

void Player::UpdateDash()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterMoveDefault(); // TODO:
		m_player_aim.lock()->EnterHidden(); // TODO
		return;
	}
	t = 1.0f - t;
	// m_transform.SetPosition(Vector3::Interpolate(m_dash_init_pos, m_dash_target_pos, t));
	Vector3 next_frame_target = Vector3::Interpolate(m_dash_init_pos, m_dash_target_pos, t);

	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	collider.velocity = next_frame_target - m_transform.GetPosition();

	// update speed level
	const Vector3& curr_pos = m_transform.GetPositionGlobal();
	const float curr_dist = (curr_pos - m_dash_init_pos).Length();
	int speed_level = GetSpeedLevelFromDistance(curr_dist);
	TryUpdateSpeedLevel(speed_level);
}

bool Player::CheckEnterBounce()
{
	auto& physics_scene = GetPhysicsScene();
	Vector3 curr_pos = m_transform.GetPositionGlobal();
	curr_pos.y += 0.5f; // TODO: center
	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	const Vector3& velocity = collider.velocity;
	Ray to_target_ray{ curr_pos, velocity.GetNormalized() };
	CollisionInfo collision_info{};
	bool hit = physics_scene.CastRayXZ(
		to_target_ray, 100.0f, ColliderLayer::MASK_BUMPER | ColliderLayer::MASK_GOAL, collision_info); // TODO
	return hit;
}

//void Player::EnterBounce()
//{
//	m_state = MoveState::BOUNCE;
//	TryUpdateSpeedLevel(m_speed_level + 1);
//}

//void Player::UpdateBounce()
//{
//	// no input? small acc?
//}

void Player::HitEffect()
{
	int se = g_global_context.m_audio_manager->LoadSoundEffect(SoundEffectName::CLEAR);
	g_global_context.m_sound->PlaySoundLoop(se, 0);

	//auto camera = m_camera.lock();
	//CameraFollow::CameraShakeConfig shake_config{ {6.0f, 6.0f, 6.0f}, 0.2f };
	//camera->SetShake(shake_config);

	auto hit_stop_updater = m_hit_stop_updater.lock();
	hit_stop_updater->SetHitStop(0.1f);
}

void Player::RotateToMoveDirection(const Vector3& move_dir)
{
	if (move_dir.LengthSquared() < Math::EPSILON)
	{
		return;
	}
	Vector3 target_fwd = move_dir;
	target_fwd.x = -target_fwd.x;
	target_fwd.Normalize();
	// quats
	// auto q_current = m_transform.GetRotation().ToXMVECTOR();
	auto up = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR q_target = DirectX::XMQuaternionRotationMatrix(
		DirectX::XMMatrixLookToLH(DirectX::g_XMZero, target_fwd.ToXMVECTOR(), up)
	);

	Vector4 rotation_target{};
	rotation_target.LoadXMVECTOR(q_target);
	m_transform.SetRotation(rotation_target);
}

int Player::GetSpeedLevelFromDistance(float distance) const
{
	return 1;
	// TODO: adjust formula
	int level = floorf(distance / m_move_config.distance_per_speed_level);
	if (level > m_move_config.speed_level_max)
	{
		level = m_move_config.speed_level_max;
	}
	return level;
}

void Player::TryUpdateSpeedLevel(int level)
{
	level = level < 0 ? 0 : (level > m_move_config.speed_level_max
		? m_move_config.speed_level_max : level);
	if (m_speed_level == level)
	{
		return;
	}

	// TODO : effect
	m_speed_level = level;
}
