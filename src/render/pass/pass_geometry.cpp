#include "pass_geometry.h"

#include <DirectXMath.h>
#include "config/constant.h"
#include "platform/graphics.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"
#include "render/dx_trace.h"

using namespace DirectX;

void PassGeometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	m_variations.clear();
	AddSubPass(std::make_unique<SubPassGeometryDefault>());
	m_default_pass_index = 0;
}

void PassGeometry::Draw()
{
	for (const auto& subpass : m_variations)
	{
		subpass->Draw();
	}
}

void PassGeometry::AddSubPass(std::unique_ptr<PassBaseGeometry> subpass)
{
	subpass->Initialize(m_device, m_context);
	m_variations.push_back(std::move(subpass));
}

void PassGeometry::ResetRenderableIndices(CameraRenderLayer layer)
{
	for (auto& subpass : m_variations)
	{
		subpass->ResetRenderableIndices(layer);
	}
}

void PassGeometry::AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc)
{
	// default subpass doesn't test if other variations exists
	// so only add to default subpass if no other variations renders this material

	// if is a deferred material
	auto pass_default = m_variations[m_default_pass_index].get();
	if (!pass_default->ShouldRender(material_desc, model_type))
	{
		return;
	}
	// try other variations
	bool rendered{ false };
	for (int index = 0; index < m_variations.size(); ++index)
	{
		if (index == m_default_pass_index)
		{
			continue;
		}
		rendered |= m_variations[index]->AddRenderableIndex(mesh_index, model_type, material_desc);
	}
	// fallback to default pass
	if (!rendered)
	{
		pass_default->AddRenderableIndex(mesh_index, model_type, material_desc);
	}
}