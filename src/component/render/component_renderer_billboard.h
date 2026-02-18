#pragma once
#include <string>
#include <vector>
#include "component/component.h"
#include "render/config/billboard_desc.h"
#include "render/config/billboard.h"

class ComponentRendererBillboard : public Component
{
public:
	void SetBillboardDesc(const BillboardDesc& desc);
	void SetSize(int size);
	void Update() override;
	const std::vector<BillboardInstance>& GetInstances() const;
	BillboardInstance& GetInstance(int index);
	const BillboardDesc& GetDesc() const;
private:
	std::vector<BillboardInstance> m_instances;
	BillboardDesc m_desc;
};