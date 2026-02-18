#include "material_desc.h"

MaterialDesc::MaterialDesc()
{
	for (auto &row : m_techniques_index)
	{
		row.fill(-1);
	}
}

void MaterialDesc::SetTechnique(TechniqueDesc desc, uint32_t mask)
{
	// add new technique
	int item_index = static_cast<int>(m_techniques.size());
	m_techniques.push_back(desc);
	// update id, and validation
	size_t tech_index = static_cast<size_t>(desc.GetType());
	for (int layer_index = 0; layer_index < static_cast<int>(CameraRenderLayer::MAX); ++layer_index)
	{
		CameraRenderLayer layer = static_cast<CameraRenderLayer>(layer_index);
		if (!CameraRenderLayerMask::IfLayerInMask(mask, layer))
		{
			continue;
		}

		int idx = m_techniques_index[layer_index][tech_index];
		// should not set multiple times for same technique + layer
		assert(idx == -1);
		m_techniques_index[layer_index][tech_index] = item_index;
	}
	// update material render layer mask
	m_render_layer_mask |= mask;
}

bool MaterialDesc::IfTechnique(RenderTechnique tech, CameraRenderLayer layer) const
{
	int index = m_techniques_index[static_cast<size_t>(layer)][static_cast<size_t>(tech)];
	return index >= 0;
}

bool MaterialDesc::IfRenderLayer(CameraRenderLayer layer) const
{
	return CameraRenderLayerMask::IfLayerInMask(m_render_layer_mask, layer);
}
