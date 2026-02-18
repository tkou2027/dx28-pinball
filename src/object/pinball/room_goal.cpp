#include "room_goal.h"
#include "component/render/component_renderer_mesh.h"
#include "component/render/component_renderer_particle.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "component/physics/component_collider.h"
// referenced objects
#include "object/pinball/room.h"
#include "object/pinball/player.h"

#include "util/debug_ostream.h"
#include "room_floor.h"
#include "field.h"

void RoomGoal::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_render);
	m_components.Add<ComponentRendererParticle>(m_comp_id_render_particle);
	m_components.Add<ComponentRendererParticle>(m_comp_id_render_particle_border);
	m_components.Add<ComponentCollider>(m_comp_id_collider);
}

void RoomGoal::Update()
{
	switch (m_state)
	{
	case GoalState::ACTIVE:
	{
		CheckUpdatePlayerSpeed();
		UpdateVisual();
		break;
	}
	case GoalState::EXIT:
	{
		UpdatePlayer();
		UpdateVisual();
		break;
	}
	case GoalState::DONE:
	{
		UpdateVisual();
		break;
	}
	}
}

void RoomGoal::InitializeConfig(const GoalConfig& config, std::weak_ptr<class Room> room)
{
	m_config = config;
	m_room = room;
	m_transform.SetParent(&room.lock()->GetTransform());
	m_transform.SetPositionX(m_config.position.x);
	m_transform.SetPositionZ(m_config.position.y);

	if (m_config.position.LengthSquared() > Math::EPSILON)
	{
		float y_rotation = std::atan2f(m_config.position.x, m_config.position.y);
		m_transform.SetRotationYOnly(y_rotation);
	}

	//m_transform.SetPositionY(0.01f); // avoid z-fighting
	InitializeVisuals();
	InitializeCollision();

	EnterIdle();
}

void RoomGoal::OnPlayerEnter(Player& player)
{
	// TODO: based on type
	player.EnterControlled();

	auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle);
	auto& particle_item = comp_render_particle.GetTextureParticleItem();
	particle_item.EnterUpdateCrush();

	auto& comp_render_particle_border = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle_border);
	auto& particle_item_border = comp_render_particle_border.GetTextureParticleItem();
	particle_item_border.EnterUpdateCrush();

	// TODO: update visual feedback
	EnterExit();
}

void RoomGoal::OnPlayerEnterFailed(const Player& player)
{
	hal::dout << "Player Enter Failed " << player.GetSpeedLevel() << std::endl;
	// TODO: update visual feedback
}

bool RoomGoal::IfDone() const
{
	return m_state == GoalState::DONE;
}

void RoomGoal::InitializeVisuals()
{
	// visual
	// range?
	//{
	//	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);

	//	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane");
	//	MaterialDesc material{};
	//	TechniqueDescDefault material_default{};
	//	material_default.use_albedo_texture = true;
	//	material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/shadow.png");
	//	material_default.no_deferred = true;
	//	material.SetTechnique(material_default);
	//	material.SetTechnique(TechniqueDescForwardUnlit{});

	//	Model model{ model_desc, material, &m_transform };
	//	const float scale = m_config.radius * 2.0f;
	//	model.GetTransform().SetScale({ scale, 1.0f, scale });
	//	model.GetTransform().SetPositionY(0.01f); // avoid z-fighting
	//	comp_render_mesh.AddModel(model);
	//}
	{
		auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);

		const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.base_color = Vector3{ 0.0f, 0.0f, 0.0f };
		material_default.no_deferred = true;
		material_default.cull_type = CullType::CULL_NONE;
		material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/bumper_icon_black.png");
		material.SetTechnique(material_default);
		material.SetTechnique(TechniqueDescForwardUnlit{});

		for (int i = 0; i < m_config.speed_level - 1; i++)
		{
			Model model{ model_desc, material, &m_transform };
			model.GetTransform().SetPositionY(2.0f + 2.0f * i);
			model.GetTransform().SetPositionZ(-1.0f);
			model.GetTransform().SetRotationYOnly(Math::PI * 2.0f + i * 0.2f);
			model.GetTransform().SetScale({ 2.0f, 2.0f });
			comp_render_mesh.AddModel(model);
		}

	}

	// screen
	{
		auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle);
		auto& particle_item = comp_render_particle.GetTextureParticleItem();
		TextureParticleConfig conf{
			TextureParticleShape::PLANE,
			256, 256
		};
		auto& texture_loader = GetTextureLoader();
		conf.texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/goal.png");

		TransformNode3D transform{};
		transform.SetParent(&m_transform);
		transform.SetPositionY(0.0f);
		transform.SetScale({ m_config.radius, m_config.radius * 4, m_config.radius });
		particle_item.SetConfig(
			conf,
			UVFrameAnimationDesc{},
			transform
		);
		particle_item.InitializeParticle();
	}

	{
		auto& comp_render_particle_border = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle_border);
		auto& particle_item = comp_render_particle_border.GetTextureParticleItem();
		TextureParticleConfig conf{
			TextureParticleShape::CYLINDER,
			64, 32
		};
		auto& texture_loader = GetTextureLoader();
		conf.texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/goal.png");

		TransformNode3D transform{};
		transform.SetParent(&m_transform);
		transform.SetPositionY(m_config.radius * 0.5f);
		transform.SetScale({ m_config.radius, m_config.radius, m_config.radius });
		particle_item.SetConfig(
			conf,
			UVFrameAnimationDesc{},
			transform
		);
		particle_item.InitializeParticle();
	}
}

void RoomGoal::InitializeCollision()
{
	// collision
	{
		auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
		Cylinder cylinder{};
		cylinder.height = m_config.radius;
		cylinder.radius = m_config.radius;
		TransformNode3D shape_transform{};
		shape_transform.SetParent(&m_transform);
		shape_transform.SetPositionY(cylinder.height * 0.5f);
		Collider goal_collider{ &m_transform, ColliderLayer::Type::GOAL };
		goal_collider.bounce = 0.3f;
		goal_collider.SetIsTrigger(false); // dynamic
		goal_collider.AddShape(ShapeCollider{ cylinder }, shape_transform);
		comp_collider.AddCollider(goal_collider);
	}
}

void RoomGoal::EnterIdle()
{
	m_state = GoalState::IDLE;
	// disable components
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(false);
	auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle);
	comp_render_particle.SetActive(false);
	auto& comp_render_particle_border = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle_border);
	comp_render_particle_border.SetActive(false);
}

void RoomGoal::EnterActive()
{
	m_state = GoalState::ACTIVE;
	// enable components
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(true);
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(true);
	auto& comp_render_particle = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle);
	comp_render_particle.SetActive(true);
	auto& comp_render_particle_border = m_components.Get<ComponentRendererParticle>(m_comp_id_render_particle_border);
	comp_render_particle_border.SetActive(true);
}

void RoomGoal::EnterExit()
{
	m_state = GoalState::EXIT;
	// disable collider
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	// countdown
	m_state_countdown.Initialize(m_action_config.player_exit_duration);
}

void RoomGoal::EnterDone()
{
	m_state = GoalState::DONE;
	// disable collider
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(false);
	auto room = m_room.lock();
	// TODO
}

void RoomGoal::CheckUpdatePlayerSpeed()
{
	// assuming player collides with at most one bumper per frame
	auto room = m_room.lock();
	const auto& player = room->GetPlayer();

	const int player_speed_level = player.GetSpeedLevel();
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	auto& collider = comp_collider.GetCollider(0);
	if (player_speed_level >= m_config.speed_level)
	{
		collider.SetIsTrigger(true);
	}
	else
	{
		collider.SetIsTrigger(false);
	}
}

void RoomGoal::UpdateVisual()
{
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	for (int i = 0; i < m_config.speed_level - 1; i++)
	{
		auto& pointer_model = comp_render.GetModel(i);
		pointer_model.GetTransform().SetRotationYDelta(Math::PI * GetDeltaTime());
	}
}

void RoomGoal::UpdatePlayer()
{
	auto room = m_room.lock();
	auto& player = room->GetPlayer();

	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();


	const Vector3 target_pos = m_transform.GetPositionGlobal();
	const float target_rotation = m_transform.GetRotationYGlobal();
	if (t <= 0.0f)
	{
		player.GetTransform().SetPosition(target_pos);
		player.GetTransform().SetRotationYOnly(target_rotation);
		player.EnterMoveDefault();
		EnterDone();
		return;
	}
	player.GetTransform().SetPosition(target_pos);
	player.GetTransform().SetRotationYOnly(target_rotation);

}
