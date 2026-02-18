#pragma once
#include "object/game_object.h"

class EnemyAttack : public GameObject
{
public:
	enum class AttackState
	{
		IDLE,
		PREPARE,
		EXECUTE,
		COOLDOWN,
		DONE
	};
	void Initialize() override;
	AttackState GetState() const { return m_state; }
private:
	AttackState m_state{ AttackState::IDLE };
};