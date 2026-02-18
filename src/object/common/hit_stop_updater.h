#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"

class HitStopUpdater : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void SetHitStop(float duration);
private:
	CountdownTimer m_timer{};
	bool m_in_hit_stop{ false };
	UpdateLayer m_layer_before_hit_stop{ UpdateLayer::DEFAULT };
};