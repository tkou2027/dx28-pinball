#include "hit_stop_updater.h"

void HitStopUpdater::Initialize()
{
	AddUpdateLayer(UpdateLayer::HIT_STOP);
}

void HitStopUpdater::Update()
{
	if (m_in_hit_stop)
	{
		m_timer.Update(GetDeltaTime());
		if (m_timer.IfDone())
		{
			m_in_hit_stop = false;
			SetSceneUpdateLayer(m_layer_before_hit_stop);
		}
	}
}

void HitStopUpdater::SetHitStop(float duration)
{
	float new_duration = Math::Max(m_timer.GetCount(), duration);
	if (new_duration < Math::EPSILON)
	{
		return;
	}
	m_in_hit_stop = true;
	m_timer.Initialize(new_duration);
	auto layer_before = GetSceneUpdateLayer();
	if (layer_before == UpdateLayer::HIT_STOP)
	{
		// already in hit stop, do not change layer
		return;
	}
	m_layer_before_hit_stop = layer_before;
	SetSceneUpdateLayer(UpdateLayer::HIT_STOP);
}
