#include "physics_scene.h"
#include "global_context.h"
#include "scene/scene_manager.h"
#include "component/physics/component_collider.h"
#include "physics/physics_system.h"
#include "physics/collision_matrix.h"

#include "util/debug_ostream.h"

namespace
{
	// collision utils
	static Vector3 SnapDirection(const Vector3& dir, float step) {
		if (Math::IsZero(dir.LengthSquared()))
		{
			return dir;
		}
		float angle = atan2f(dir.x, dir.z);
		float snapped = roundf(angle / step) * step;
		float length = dir.Length();
		return Vector3(sinf(snapped) * length, 0.0f, cosf(snapped) * length);
	}
}

void PhysicsScene::Initialize() {}
void PhysicsScene::Finalize() {}
void PhysicsScene::Update()
{
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	if (!scene)
	{
		m_component_manager = nullptr;
		return;
	}
	m_component_manager = &scene->GetObjectList().GetComponentManager();
	m_update_layer = scene->GetObjectList().GetUpdateLayer();
	UpdateSceneData();
	UpdateActors();
}

void PhysicsScene::SetCollisionMatrix(std::weak_ptr<class CollisionMatrix> collision_matrix)
{
	m_collision_matrix = collision_matrix;
}

void PhysicsScene::UpdateSceneData()
{
	assert(m_component_manager);
	// TODO: partial update
	m_actors.clear();
	m_solids.clear();
	m_triggers.clear();

	const auto& collider_comps = m_component_manager->GetComponents<ComponentCollider>();
	for (const auto& collider_comp : collider_comps)
	{
		if (!collider_comp.GetActive())
		{
			continue;
		}
		const auto& object = collider_comp.GetOwner().lock();
		if (!object->IfUpdateLayer(m_update_layer))
		{
			continue;
		}

		const auto& colliders = collider_comp.GetColliders();
		// push back
		for (const auto& collider : colliders)
		{
			if (collider.GetIsActor())
			{
				m_actors.push_back(collider);
			}
			else if (collider.GetIsTrigger())
			{
				m_triggers.push_back(collider);
			}
			else
			{
				m_solids.push_back(collider);
			}
		}
	}
	//// add actors
	//const auto& actor_comps = m_component_manager->GetComponents<ComponentColliderActor>();
	//for (const auto& actor_comp : actor_comps)
	//{
	//	if (!actor_comp.GetActive())
	//	{
	//		continue;
	//	}
	//	const auto& colliders = actor_comp.GetColliders();
	//	m_actors.insert(std::end(m_actors), std::begin(colliders), std::end(colliders));
	//}
}

void PhysicsScene::UpdateActors()
{
	assert(m_component_manager);
	std::vector<CollisionInfo> collisions;
	for (auto& actor : m_actors)
	{
		UpdateActor(actor, collisions);
	}
	// update back to components TODO: better management
	for (auto& actor : m_actors)
	{
		auto& actor_comp = m_component_manager->GetComponent<ComponentCollider>(actor.GetComponentId());
		actor_comp.GetCollider(actor.GetIndexInComponent()).velocity = actor.velocity;
	}
	// collision callbacks
	for (auto& collision : collisions)
	{
		auto& actor_comp = m_component_manager->GetComponent<ComponentCollider>(collision.self->GetComponentId());
		auto object_ptr = actor_comp.GetOwner().lock();
		if (collision.is_trigger)
		{
			object_ptr->OnTrigger(collision);
		}
		else
		{
			object_ptr->OnCollision(collision);
		}
	}
}

void PhysicsScene::UpdateActor(Collider& actor, std::vector<CollisionInfo>& collisions)
{
	// TODO: gravity
	actor.velocity += Vector3(0.0f, -0.01f, 0.0f) * actor.gravity_scale; // gravity
	Vector3 movement_left = actor.velocity;

	auto collision_matrix = m_collision_matrix.lock();
	std::set<Collider*> triggered_this_frame;
	// update movement
	const int MAX_ITERATIONS{ 4 };
	for (int iter = 0; iter < MAX_ITERATIONS; ++iter)
	{
		if (Math::IsZero(movement_left.LengthSquared()))
		{
			break;
		}

		HitInfo closest_hit{};
		Collider* closest_solid = nullptr;

		// Get nearest collision
		for (auto& solid : m_solids)
		{
			if (!collision_matrix->IfCollides(actor.GetLayer(), solid.GetLayer()))
			{
				continue;
			}
			HitInfo tmp_hit{};
			if (actor.IfCollideSwept(solid, movement_left, tmp_hit))
			{
				if (!closest_hit.hit || tmp_hit.t < closest_hit.t)
				{
					closest_hit = tmp_hit;
					closest_solid = &solid;
				}
			}
		}

		// resolve collision
		if (closest_hit.hit)
		{
			collisions.push_back(CollisionInfo{ true, false, closest_hit, closest_solid, &actor });
			// fix already overlapping
			if (closest_hit.penetration >= 0.0f)
			{
				Vector3 push_out = closest_hit.hit_normal * (closest_hit.penetration + 0.001f);
				UpdateActorTriggers(actor, push_out, collisions, triggered_this_frame);
				actor.GetTransformParent()->SetPositionDelta(push_out);
				continue;
			}

			// update moved
			float t_safe = Math::Max(0.0f, closest_hit.t - 0.001f);
			Vector3 movement_to_hit = movement_left * t_safe;
			UpdateActorTriggers(actor, movement_to_hit, collisions, triggered_this_frame);

			// resolve collision
			actor.GetTransformParent()->SetPositionDelta(movement_to_hit);
			Vector3 leftover = movement_left * (1.0f - closest_hit.t);
			float dot = leftover.Dot(closest_hit.hit_normal);
			if (dot <= 0.0f)
			{
				if (closest_hit.hit_normal.y > 0.7f)
				{
					leftover = leftover - closest_hit.hit_normal * dot;
					actor.velocity.y = 0.0f;
				}
				else
				{
					closest_hit.hit_normal.y = 0.0f;
					float bounce = closest_solid->bounce;
					leftover = leftover - closest_hit.hit_normal * (1.0f + bounce) * dot;
					actor.velocity = actor.velocity - closest_hit.hit_normal * (1.0f + bounce) * actor.velocity.Dot(closest_hit.hit_normal);


					//	if (closest_solid->GetSnapReflection())
					//	{
					//		float step = closest_solid->GetSnapReflectionStep();
					//		movement_left = SnapDirection(movement_left, step);
					//		actor.velocity = SnapDirection(actor.velocity, step);
					//	}
				}
			}
			movement_left = leftover;
		}
		else
		{
			UpdateActorTriggers(actor, movement_left, collisions, triggered_this_frame);
			actor.GetTransformParent()->SetPositionDelta(movement_left);
			break;
		}
	}
}

void PhysicsScene::UpdateActorTriggers(Collider& actor, const Vector3& movement,
	std::vector<CollisionInfo>& collisions, std::set<Collider*>& triggered_set)
{
	auto collision_matrix = m_collision_matrix.lock();

	for (auto& trigger : m_triggers)
	{
		if (!collision_matrix->IfCollides(actor.GetLayer(), trigger.GetLayer()))
		{
			continue;
		}
		if (triggered_set.find(&trigger) != triggered_set.end())
		{
			continue;
		}
		HitInfo trigger_hit{};
		// TODO: easier hit
		if (actor.IfCollideSwept(trigger, movement, trigger_hit))
		{
			triggered_set.insert(&trigger);
			CollisionInfo info{ true, true, trigger_hit, &trigger, &actor };
			collisions.push_back(info);
		}
	}
}

GameObject& PhysicsScene::GetObjectOfCollider(const Collider& collider)
{
	assert(m_component_manager);
	auto& collider_comp = m_component_manager->GetComponent<ComponentCollider>(collider.GetComponentId());
	auto object_ptr = collider_comp.GetOwner().lock();
	return *object_ptr;
}

bool PhysicsScene::CastRayXZ(const Ray& ray, float max_distance, uint32_t layer_mask, CollisionInfo& collision_info) const
{
	// TODO: actors?
	for (const auto& solid : m_solids)
	{
		if (!m_collision_matrix.lock()->IfInMask(layer_mask, solid.GetLayer()))
		{
			continue;
		}
		HitInfo hit_info{};
		if (solid.IfRayHitXZ(ray, max_distance, hit_info))
		{
			if (!collision_info.hit || hit_info.t < collision_info.hit_info.t)
			{
				collision_info.hit = true;
				collision_info.hit_info = hit_info;
				collision_info.other = &solid;
			}
		}
	}
	for (const auto& trigger : m_triggers)
	{
		if (!m_collision_matrix.lock()->IfInMask(layer_mask, trigger.GetLayer()))
		{
			continue;
		}
		HitInfo hit_info{};
		if (trigger.IfRayHitXZ(ray, max_distance, hit_info))
		{
			if (!collision_info.hit || hit_info.t < collision_info.hit_info.t)
			{
				collision_info.hit = true;
				collision_info.hit_info = hit_info;
				collision_info.other = &trigger;
			}
		}
	}
	return collision_info.hit;
}


bool PhysicsScene::ClosestHit(const Vector3& origin, float max_distance, uint32_t layer_mask, CollisionInfo& collision_info) const
{
	float curr_distance = max_distance;
	for (const auto& solid : m_solids)
	{
		if (!m_collision_matrix.lock()->IfInMask(layer_mask, solid.GetLayer()))
		{
			continue;
		}
		Vector3 min_position{};
		float distance = solid.GetDistance(origin, min_position);
		if (distance < curr_distance)
		{
			curr_distance = distance;
			collision_info.hit = true;
			collision_info.other = &solid;
			collision_info.hit_info.hit_position = min_position;
			collision_info.hit_info.t = curr_distance;
		}
	}
	for (const auto& trigger : m_triggers)
	{
		if (!m_collision_matrix.lock()->IfInMask(layer_mask, trigger.GetLayer()))
		{
			continue;
		}
		Vector3 min_position{};
		float distance = trigger.GetDistance(origin, min_position);
		if (distance < curr_distance)
		{
			curr_distance = distance;
			collision_info.hit = true;
			collision_info.other = &trigger;
			collision_info.hit_info.hit_position = min_position;
			collision_info.hit_info.t = curr_distance;
		}
	}
	return collision_info.hit;
}


bool PhysicsScene::CastRayXZ(const Ray& ray, float max_distance, const Collider& other, CollisionInfo& collision_info) const
{
	HitInfo hit_info{};
	if (other.IfRayHitXZ(ray, max_distance, hit_info))
	{
		if (!collision_info.hit || hit_info.t < collision_info.hit_info.t)
		{
			collision_info.hit = true;
			collision_info.hit_info = hit_info;
			collision_info.other = &other;
		}
	}
	return hit_info.hit;
}
