#pragma once

class Countdown
{
public:
	Countdown(int interval = 0) : m_interval(interval) {}
	void Initialize(int interval)
	{
		m_interval = interval;
		m_count = interval;
	}
	void Update()
	{
		if (m_count <= 0)
		{
			m_count = 0;
			return;
		}
		m_count--;
	}
	int GetCount() const
	{
		return m_count;
	}
	float GetT() const
	{
		return m_interval <= 0 ? 0.0f :
			static_cast<float>(m_count) / m_interval;
	}
private:
	int m_interval{ 0 };
	int m_count{ 0 };
};