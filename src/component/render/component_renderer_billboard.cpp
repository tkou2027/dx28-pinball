#include "component_renderer_billboard.h"

void ComponentRendererBillboard::SetBillboardDesc(const BillboardDesc& desc)
{
	m_desc = desc;
}

void ComponentRendererBillboard::SetSize(int size)
{
	m_instances.resize(size);
}

void ComponentRendererBillboard::Update()
{
	for (auto& instance : m_instances)
	{
		instance.Update(m_desc);
	}
}

const std::vector<BillboardInstance>& ComponentRendererBillboard::GetInstances() const
{
	return m_instances;
}

BillboardInstance& ComponentRendererBillboard::GetInstance(int index)
{
	return m_instances.at(index);
}

const BillboardDesc& ComponentRendererBillboard::GetDesc() const
{
	return m_desc;
}
