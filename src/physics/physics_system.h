#pragma once
#include <memory>

class PhysicsScene;
class CollisionMatrix;
class PhysicsSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();
	void Initialize();
	void Finalize();
	void Update();
	const PhysicsScene& GetPhysicsScene() const { return *m_physics_scene; }
	const CollisionMatrix& GetCollisionMatrix() const { return *m_collision_matrix; }
private:
	std::unique_ptr<PhysicsScene> m_physics_scene{};
	std::shared_ptr<CollisionMatrix> m_collision_matrix{};
};