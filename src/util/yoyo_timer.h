#pragma once

class YoyoTimer
{
public:
	YoyoTimer(float interval = 0.0f) : m_interval(interval) {}
	void Initialize(float interval)
	{
		m_interval = interval;
		m_count = 0;
		m_direction = 1;
	}
	float Update(float delta_time)
	{
		if (m_direction > 0.0f && m_count >= m_interval)
		{
			m_direction = -m_direction;
		}
		else if (m_direction < 0.0f && m_count <= 0.0f)
		{
			m_direction = -m_direction;
		}
		m_count += m_direction * delta_time;
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
private:
	float m_interval{ 0.0f };
	float m_count{ 0.0f };
	int m_direction{ 1 };
};