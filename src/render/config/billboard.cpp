#include "billboard.h"

void BillboardInstance::Update(const BillboardDesc& desc)
{
	if (!m_active)
	{
		return;
	}
	// uv animation
	m_uv_animation_state.Update(desc.uv_animation_desc);
}

Vector3 BillboardInstance::GetPosition() const
{
	if (!m_transform_parent)
	{
		return m_position;
	}
	return m_transform_parent->GetPositionGlobal() + m_position;
}

Vector2 BillboardInstance::GetSize() const
{
	return m_size;
}

const UVRect& BillboardInstance::GetUV(const BillboardDesc& desc) const
{
	return m_uv_animation_state.GetUVRect(desc.uv_animation_desc);
}