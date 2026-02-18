#pragma once

class Yoyo
{
public:
	Yoyo(int interval) : m_interval(interval) {}
	void Initialize(int interval)
	{
		m_interval = interval;
		m_count = 0;
		m_direction = 1;
	}
	void Update()
	{
		if (m_direction > 0 && m_count >= m_interval)
		{
			m_direction = -m_direction;
		}
		else if (m_direction < 0 && m_count <= 0)
		{
			m_direction = -m_direction;
		}
		m_count += m_direction;
	}
	int GetCount() const
	{
		return m_count;
	}
	float GetT() const
	{
		return (float)m_count / m_interval; // TODO: assert not zero
	}
private:
	int m_interval{ 0 };
	int m_count{ 0 };
	int m_direction{ 1 };
};