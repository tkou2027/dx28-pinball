#pragma once
// countdown but float
class CountdownTimer
{
public:
	CountdownTimer(float interval = 0.0f) : m_interval(interval) {}
	void Initialize(float interval)
	{
		m_interval = interval;
		m_count = interval;
	}
	float Update(float delta_time)
	{
		m_count -= delta_time;
		if (m_count <= 1e-6)
		{
			m_count = 0.0f;
		}
		return GetT();
	}
	float GetCount() const
	{
		return m_count;
	}
	float GetT() const
	{
		return m_interval <= 1e-6 ? 0.0f : m_count / m_interval;
	}
	float GetInterval() const
	{
		return m_interval;
	}
	bool IfDone() const
	{
		return m_count <= 1e-6;
	}
private:
	float m_interval{ 0.0f };
	float m_count{ 0.0f };
};