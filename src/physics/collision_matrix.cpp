#include "collision_matrix.h"

void CollisionMatrix::Initialize()
{
	for (size_t i = 0; i < static_cast<size_t>(ColliderLayer::Type::MAX); ++i)
	{
		m_matrix[i] = 0;
	}
	SetLayerCollision(ColliderLayer::Type::PLAYER, ColliderLayer::Type::GROUND, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER, ColliderLayer::Type::BORDER, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER, ColliderLayer::Type::BUMPER_LOCKON, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER, ColliderLayer::Type::BUMPER, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER, ColliderLayer::Type::GOAL, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER, ColliderLayer::Type::ENEMY, true);

	SetLayerCollision(ColliderLayer::Type::PLAYER_DASH, ColliderLayer::Type::GROUND, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER_DASH, ColliderLayer::Type::BORDER, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER_DASH, ColliderLayer::Type::BUMPER_LOCKON, true);
	SetLayerCollision(ColliderLayer::Type::PLAYER_DASH, ColliderLayer::Type::ENEMY, true);
}