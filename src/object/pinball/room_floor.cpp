#include "room_floor.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "render/render_resource.h"

#include "object/game_object_list.h"
#include "object/pinball/room_floor_visual.h"
// physics
#include "component/physics/component_collider.h"
// other objects
#include "object/pinball/room.h"
// utils
#include "util/tween.h"

void RoomFloor::Initialize()
{
	m_components.Add<ComponentCollider>(m_comp_id_collider);
	m_visual = GetOwner().CreateGameObject<RoomFloorVisual>();
	m_visual.lock()->GetTransform().SetParent(&m_transform);
}

void RoomFloor::InitializeConfig(const RoomConfig& config, std::weak_ptr<Room> room)
{
	m_config = config;
	m_room = room;
	m_transform.SetParent(&room.lock()->GetTransform());
	InitializeVisuals();
	InitializeCollision();
	EnterIdle();
}

void RoomFloor::Update()
{
	switch (m_state)
	{
	case FloorState::TRANS_IN:
	{
		UpdateTransIn();
		break;
	}
	case FloorState::TRANS_OUT:
	{
		UpdateTransOut();
		break;
	}
	}
}

void RoomFloor::EnterIdle()
{
	m_state = FloorState::IDLE;
	// disable components
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	//auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//comp_render.SetActive(false);
}

void RoomFloor::EnterTransIn(float duration)
{
	m_state = FloorState::TRANS_IN;
	m_state_countdown.Initialize(duration);
	// enable visual
	auto visual = m_visual.lock();
	visual->SetVisible(true);
	//comp_render.SetActive(true);
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(true);
}

void RoomFloor::EnterActive()
{
	m_state = FloorState::ACTIVE;
	// enable all components
	//auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	//comp_collider.SetActive(true);

	//auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//comp_render.SetActive(true);

	// reset height
	//auto& render_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//auto& models = render_comp.GetModels();
	//for (auto& model : models)
	//{
	//	model.GetTransform().SetPositionY(0.0f);
	//}
}

void RoomFloor::EnterTransOut(float duration)
{
	m_state = FloorState::TRANS_OUT;
	m_state_countdown.Initialize(duration);
}

void RoomFloor::EnterDone()
{
	m_state = FloorState::DONE;
	// disable collision
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
}

void RoomFloor::InitializeVisuals()
{
	auto visual = m_visual.lock();
	visual->InitializeConfig(m_config.floor);
	visual->SetVisible(false);
}

void RoomFloor::InitializeCollision()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	//{
	//	PolygonRing floor_circle{};
	//	floor_circle.radius = m_config.floor.radius_outer;
	//	floor_circle.radius_inner = m_config.floor.radius_inner;
	//	floor_circle.sides = 4;
	//	// floor_circle.rotation_y = Math::PI * 0.25f;
	//	TransformNode3D transform{};
	//	transform.SetParent(&m_transform);
	//	transform.SetRotationYOnly(Math::PI * 0.25f);
	//	Collider floor_collider{ &m_transform, ColliderLayer::Type::GROUND };
	//	floor_collider.AddShape(ShapeCollider{ floor_circle }, transform);
	//	comp_collider.AddCollider(floor_collider);
	//}

	//{
	//	// TODO: border colliders
	//	CylinderSideInverse floor_border{};
	//	floor_border.radius = m_config.floor.radius_outer;
	//	floor_border.height = 30.0f;
	//	TransformNode3D transform{};
	//	transform.SetParent(&m_transform);
	//	Collider floor_collider{ &m_transform, ColliderLayer::Type::BORDER };
	//	floor_collider.AddShape(ShapeCollider{ floor_border }, transform);
	//	comp_collider.AddCollider(floor_collider);
	//}

	// tmp floor
	//{
	//	Plane floor_plane{};
	//	TransformNode3D transform{};
	//	transform.SetParent(&m_transform);
	//	transform.SetRotationEuler({-Math::PI * 0.5f, 0.0f, 0.0f});
	//	Collider floor_collider{ &m_transform, ColliderLayer::Type::GROUND };
	//	floor_collider.AddShape(ShapeCollider{ floor_plane }, transform);
	//	comp_collider.AddCollider(floor_collider);
	//}
	// border
	for (int i = 0; i < 4; i++)
	{
		Plane border_plane{};
		TransformNode3D transform{};
		transform.SetParent(&m_transform);
		float rotation = Math::HALF_PI * i;
		Vector3 move_dir{ sinf(rotation), 0.0f, cosf(rotation) };
		transform.SetRotationYOnly(rotation + Math::PI);
		transform.SetPosition(move_dir * m_config.floor.radius_outer * (1.0f / 1.4f));
		Collider border_collider{ &m_transform, ColliderLayer::Type::BORDER };
		border_collider.AddShape(ShapeCollider{ border_plane }, transform);
		border_collider.bounce = 0.2f;
		comp_collider.AddCollider(border_collider);
	}
	// floor
	for (int i = 0; i < 4; i++)
	{
		PolygonCylinder floor_block{};
		floor_block.sides = 4;
		floor_block.radius = m_config.floor.radius_outer;
		floor_block.height = m_config.floor.side_height;
		TransformNode3D transform{};
		transform.SetParent(&m_transform);
		float rotation = Math::HALF_PI * i;
		transform.SetRotationYOnly(Math::PI * 0.25f);
		transform.SetPositionY(-m_config.floor.side_height * 0.5f);
		Vector3 move_dir{ sinf(rotation), 0.0f, cosf(rotation) };
		transform.SetPositionDelta(move_dir * (
			m_config.floor.radius_outer * Math::SQRT_2 - 
			(m_config.floor.radius_outer - m_config.floor.radius_inner) * Math::INV_SQRT_2
		));
		Collider border_collider{ &m_transform, ColliderLayer::Type::GROUND };
		border_collider.AddShape(ShapeCollider{ floor_block }, transform);
		comp_collider.AddCollider(border_collider);
	}
}

void RoomFloor::UpdateTransIn()
{
	// update countdown
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterActive();
		return;
	}
	// update fall
	const float y_start = m_config.trans_in_height_offset;
	const float y_end = 0.0f;
	if (!UpdateBlockTransInOut(y_start, y_end, t))
	{
		EnterActive();
	}
}

void RoomFloor::UpdateTransOut()
{
	// update countdown
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterDone();
		return;
	}
	// update fall
	const float y_start = 0.0f;
	const float y_end = m_config.trans_out_height_offset;
	if (!UpdateBlockTransInOut(y_start, y_end, t))
	{
		EnterDone();
	}
}

bool RoomFloor::UpdateBlockTransInOut(float y_start, float y_end, float t)
{
	//// update fall
	//auto& render_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//auto& models = render_comp.GetModels();
	//const int models_count = static_cast<int>(models.size());
	//if (models_count == 0)
	//{
	//	return false;
	//}

	//t = 1.0f - t;
	//t = Tween::EaseFunc(Tween::TweenFunction::EASE_IN_QUAD, t);
	//// try to delay 2% per model
	//const float fall_duration = 1.0f;//Math::Max(0.8f, 1.0f - models_count * 0.02f);
	//const float fall_delay_per_model = (1.0f - fall_duration) / models_count;

	//for (int i = 0; i < models_count; i++)
	//{
	//	auto& model = models[i];
	//	const float model_t = Math::Clamp(t - fall_delay_per_model * i, 0.0f, fall_duration) / fall_duration;
	//	float y = Math::Lerp(y_start, y_end, model_t);
	//	model.GetTransform().SetPositionY(y);
	//}
	return true;
}

