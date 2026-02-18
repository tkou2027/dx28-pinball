#include "pass_base_geometry.h"

#include <DirectXMath.h>

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_common.h"

using namespace DirectX;

void PassBaseGeometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	m_vertex_common.Initialize(m_device);
}

void PassBaseGeometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, VertexCommon::VertexCommonConfig config)
{
	PassBase::Initialize(device, context);
	m_vertex_common.Initialize(m_device, config);
}

void PassBaseGeometry::Draw()
{
	SetInfoPerDraw();

	const auto& renderable_manager = GetRenderScene().GetRenderablesManager();
	const auto& material_resource = GetMaterialResourceManager();

	// static
	if (SetInfoPerVertexShader(ModelType::STATIC))
	{
		for (const auto& index : m_mesh_indices_static)
		{
			const auto& model = renderable_manager.m_models.at(index);
			const auto& material_desc = material_resource.GetMaterialDesc(model.key.material_id);
			// if (!ShouldRender(material_desc))
			// {
			// 	continue;
			// }
			SetInfoPerModel(model);
			m_vertex_common.DrawModelStatic(m_context, model, m_vertex_common.GetInputLayoutType(ModelType::STATIC));
		}
	}

	// instancing
	if (SetInfoPerVertexShader(ModelType::INSTANCED))
	{
		std::vector<ModelRenderInfo> batch_models{};
		batch_models.reserve(m_mesh_indices_instanced.size());
		for (const auto& index : m_mesh_indices_instanced)
		{
			const auto& model = renderable_manager.m_models.at(index);
			batch_models.push_back(model);
		}

		// TODO
		for (size_t i = 0; i < batch_models.size(); )
		{
			const auto current_material = batch_models[i].key.material_id;
			const auto current_model = batch_models[i].key.model_id;

			size_t j = i;
			for (; j < batch_models.size(); ++j)
			{
				if (batch_models[j].key.material_id != current_material ||
					batch_models[j].key.model_id != current_model)
				{
					break;
				}
			}

			SetInfoPerMaterial(batch_models[i].key);

			// call vertex common instanced draw
			m_vertex_common.DrawModelInstanced(m_context, batch_models, i, j, m_vertex_common.GetInputLayoutType(ModelType::INSTANCED));

			// advance to next group
			i = j;
		}
	}
	// skinned
	if (SetInfoPerVertexShader(ModelType::SKINNED))
	{
		
		for (const auto& index : m_mesh_indices_skinned)
		{
			const auto& model = renderable_manager.m_models.at(index);
			const auto& material_desc = material_resource.GetMaterialDesc(model.key.material_id);
			// if (!ShouldRender(material_desc))
			// {
			// 	continue;
			// }
			SetInfoPerModel(model);
			m_vertex_common.DrawModelSkinned(m_context, model, m_vertex_common.GetInputLayoutType(ModelType::SKINNED));
		}
	}
}

void PassBaseGeometry::ResetRenderableIndices(CameraRenderLayer layer)
{
	m_mesh_indices_static.clear();
	m_mesh_indices_skinned.clear();
	m_mesh_indices_instanced.clear();
	// m_mesh_indices_instanced.reserve(4096);
	m_curr_render_layer = layer;
}

bool PassBaseGeometry::AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc)
{
	if (!ShouldRender(material_desc, model_type))
	{
		return false;
	}
	switch (model_type)
	{
	case ModelType::STATIC:
	{
		m_mesh_indices_static.push_back(mesh_index);
		return true;
	}
	case ModelType::SKINNED:
	{
		m_mesh_indices_skinned.push_back(mesh_index);
		return true;
	}
	case ModelType::INSTANCED:
	{
		m_mesh_indices_instanced.push_back(mesh_index);
		return true;
	}
	}
	return false;
}

void PassBaseGeometry::SetInfoPerDraw()
{
	// view
	const auto& render_resource = GetRenderResource();
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->VSSetConstantBuffers(2, 1, render_resource.m_buffer_per_mesh.GetAddressOf());
}

bool PassBaseGeometry::SetInfoPerVertexShader(ModelType type)
{
	return m_vertex_common.SetVertexShaderAndInputLayout(m_context, type);
}

void PassBaseGeometry::SetInfoPerModel(const ModelRenderInfo& model)
{
	BufferPerMesh cb{};
	XMMATRIX world = XMLoadFloat4x4(&model.instance.model_matrix);
	XMStoreFloat4x4(&cb.matrix_model, XMMatrixTranspose(world));
	XMStoreFloat4x4(&cb.matrix_model_inverse_transpose, XMMatrixInverse(nullptr, world));
	cb.uv_offset = XMFLOAT2(model.instance.uv_offset.x, model.instance.uv_offset.y);
	cb.uv_size = XMFLOAT2(model.instance.uv_size.x, model.instance.uv_size.y);

	// 定数バッファに行列をセット
	const auto& render_resource = GetRenderResource();
	m_context->UpdateSubresource(render_resource.m_buffer_per_mesh.Get(), 0, nullptr, &cb, 0, 0);
}