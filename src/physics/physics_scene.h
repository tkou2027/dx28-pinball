#pragma once
#include <set>
#include <vector>
#include <memory>
#include "math/ray.h"
#include "physics/collider.h"
#include "object/update_layer.h"

struct CollisionInfo
{
	bool hit{ false };
	bool is_trigger{ false };
	HitInfo hit_info{};
	const Collider* other{ nullptr };
	const Collider* self{ nullptr };
};

class GameObject;
class ComponentManager;
class CollisionMatrix;
class PhysicsScene
{
public:
	void Initialize();
	void Finalize();
	void Update();
	void SetCollisionMatrix(std::weak_ptr<CollisionMatrix> collision_matrix);
	bool CastRayXZ(
		const Ray& ray, float max_distance, uint32_t layer_mask, CollisionInfo& collision_info) const;
	bool CastRayXZ(
		const Ray& ray, float max_distance, const Collider& other, CollisionInfo& collision_info) const;
	bool ClosestHit(const Vector3& origin, float max_distance, uint32_t layer_mask, CollisionInfo& collision_info) const;
private:
	void UpdateSceneData();
	void UpdateActors();
	void UpdateActor(Collider& actor, std::vector<CollisionInfo>& collisions);
	void UpdateActorTriggers(Collider& actor, const Vector3& movement,
		std::vector<CollisionInfo>& collisions, std::set<Collider*>& triggered_set);
	GameObject& GetObjectOfCollider(const Collider& collider);

	std::vector<Collider> m_actors;
	std::vector<Collider> m_solids;
	std::vector<Collider> m_triggers;
	std::weak_ptr<CollisionMatrix> m_collision_matrix;
	ComponentManager* m_component_manager{};
	UpdateLayer m_update_layer{ UpdateLayer::DEFAULT };
};