#pragma once

class PoolObject
{
public:
	virtual ~PoolObject() = default;
	virtual void Update() {}
	bool IfUse() const { return m_use; }
	void SetUse(bool use) { m_use = use; }
protected:
	bool m_use{ false };
};
