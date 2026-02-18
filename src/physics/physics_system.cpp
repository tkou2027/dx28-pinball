#include "physics_system.h"
#include "physics_scene.h"
#include "collision_matrix.h"

PhysicsSystem::PhysicsSystem() = default;
PhysicsSystem::~PhysicsSystem() = default;

void PhysicsSystem::Initialize()
{
    m_physics_scene = std::make_unique<PhysicsScene>();
    m_physics_scene->Initialize();
	m_collision_matrix = std::make_unique<CollisionMatrix>();
	m_collision_matrix->Initialize();
	m_physics_scene->SetCollisionMatrix(m_collision_matrix);
}

void PhysicsSystem::Finalize()
{
    m_physics_scene->Finalize();
}

void PhysicsSystem::Update()
{
    m_physics_scene->Update();
}
