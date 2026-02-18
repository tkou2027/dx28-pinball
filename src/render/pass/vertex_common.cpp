#include "vertex_common.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_common.h"

#include <chrono>
#include "util/debug_ostream.h"

using namespace DirectX;

void VertexCommon::Initialize(ID3D11Device* device)
{
	m_vertex_inputs[static_cast<size_t>(ModelType::STATIC)] =
		Shader::CreateShaderVertex(device,
			L"vertex_geometry_static.cso", Shader::InputLayoutType::MESH_STATIC);
	m_vertex_inputs[static_cast<size_t>(ModelType::SKINNED)] =
		Shader::CreateShaderVertex(device,
			L"vertex_geometry_skinned.cso", Shader::InputLayoutType::MESH_SKINNED);
	m_vertex_inputs[static_cast<size_t>(ModelType::INSTANCED)] =
		Shader::CreateShaderVertex(device,
			L"vertex_geometry_instanced.cso", Shader::InputLayoutType::MESH_STATIC);
}

void VertexCommon::Initialize(ID3D11Device* device, const VertexCommonConfig& config)
{
	for (int i = 0; i < static_cast<int>(ModelType::MAX); ++i)
	{
		const auto& shader_config = config.shader_configs[i];
		if (shader_config.shader_name.empty())
		{
			continue;
		}
		m_vertex_inputs[i] = Shader::CreateShaderVertex(device,
			shader_config.shader_name, shader_config.input_layout_type);
	}
}

bool VertexCommon::SetVertexShaderAndInputLayout(ID3D11DeviceContext* context, ModelType type)
{
	const auto& vertex_input = m_vertex_inputs[static_cast<size_t>(type)];
	if (!vertex_input.vertex_shader)
	{
		// not supported
		return false;
	}
	// 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
	context->VSSetShader(vertex_input.vertex_shader.Get(), nullptr, 0);
	// 頂点レイアウトを描画パイプラインに設定
	context->IASetInputLayout(vertex_input.input_layout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ?

	return true;
}

void VertexCommon::DrawModelStatic(ID3D11DeviceContext* context, ModelRenderInfo model_key, Shader::InputLayoutType input_layout_type)
{
	const auto& model_data = GetModelLoader().GetModel(model_key.key.model_id);
	for (const auto& mesh_data : model_data.meshes)
	{
		// TODO: binding based on material
		if (model_data.textures.size() > 0)
		{
			context->PSSetShaderResources(0, 1, model_data.textures[mesh_data.m_material_index].m_diffuse.GetAddressOf());
			context->PSSetShaderResources(3, 1, model_data.textures[mesh_data.m_material_index].m_metallic.GetAddressOf());
		}

		SetupVertexBuffer(context, mesh_data, input_layout_type);
		context->IASetIndexBuffer(
			mesh_data.m_indices.Get(),
			mesh_data.m_index_count > 65535 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
		context->DrawIndexed(static_cast<UINT>(mesh_data.m_index_count), 0, 0);
	}
}

void VertexCommon::DrawModelSkinned(
	ID3D11DeviceContext* context, ModelRenderInfo render_key, Shader::InputLayoutType input_layout_type)
{
	const auto& model_loader = GetModelLoader();
	const auto& model_data = model_loader.GetModel(render_key.key.model_id);
	const auto& model_data_skinned = model_loader.GetModelSkinned(render_key.key.model_skinning_id);
	const auto& model_data_skinned_matrix = model_loader.GetModelSkinnedMatrix(render_key.key.model_skinning_matrix_id);

	// set bone matrix srv
	context->VSSetShaderResources(0, 1, model_data_skinned_matrix.bone_matrix_srv.GetAddressOf());
	int mesh_index{ 0 };
	for (const auto& mesh_data : model_data.meshes)
	{
		// TODO: binding based on material
		if (model_data.textures.size() > 0)
		{
			context->PSSetShaderResources(0, 1, model_data.textures[mesh_data.m_material_index].m_diffuse.GetAddressOf());
		}
		//ID3D11Buffer* pVBs[] = { mesh_data.m_positions.Get(), mesh_data.m_normals.Get(),
		//	mesh_data.m_pTexcoordArrays.size() > 0 ? mesh_data.m_pTexcoordArrays[0].Get() : nullptr,
		//	 model_data_skinned.meshes_skinned[mesh_index].bone_ids.Get(),  model_data_skinned.meshes_skinned[mesh_index].bone_weights.Get() };
		//uint32_t strides[] = { 12, 12, 8, 16, 16 };
		//uint32_t offsets[] = { 0, 0, 0, 0, 0 };
		// context->IASetVertexBuffers(0, ARRAYSIZE(pVBs), pVBs, strides, offsets);
		SetupVertexBuffer(context, mesh_data, model_data_skinned.meshes_skinned[mesh_index], input_layout_type);
		context->IASetIndexBuffer(
			mesh_data.m_indices.Get(),
			mesh_data.m_index_count > 65535 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
		context->DrawIndexed(static_cast<UINT>(mesh_data.m_index_count), 0, 0);
		mesh_index++;
	}
}

void VertexCommon::DrawModelInstanced(
	ID3D11DeviceContext* context, const std::vector<ModelRenderInfo> models,
		unsigned int start_index, unsigned int end_index_exclude, Shader::InputLayoutType input_layout_type)
{
	if (models.empty() || end_index_exclude <= start_index)
	{
		return;
	}
	auto start = std::chrono::high_resolution_clock::now();

	const unsigned int num_models = static_cast<unsigned int>(end_index_exclude - start_index);
	assert(end_index_exclude <= models.size() && start_index >= 0);
	const ModelRenderKey& render_key = models[start_index].key;

	// prepare instance buffer data
	// compute matrix
	std::vector <BufferPerMesh> instance_payload{};
	instance_payload.resize(num_models);
	for (int i = start_index; i < end_index_exclude; i++)
	{
		const int payload_index = i - start_index;
		XMMATRIX model_matrix = XMLoadFloat4x4(&models[i].instance.model_matrix);
		XMFLOAT4X4 matrix_model{};
		XMFLOAT4X4 matrix_model_inverse_transpose{};
		XMStoreFloat4x4(&instance_payload[payload_index].matrix_model, XMMatrixTranspose(model_matrix));
		XMStoreFloat4x4(&instance_payload[payload_index].matrix_model_inverse_transpose, XMMatrixInverse(nullptr, model_matrix));
		instance_payload[payload_index].uv_offset = XMFLOAT2(models[i].instance.uv_offset.x, models[i].instance.uv_offset.y);
		instance_payload[payload_index].uv_size = XMFLOAT2(models[i].instance.uv_size.x, models[i].instance.uv_size.y);
	}

	const auto& model_data = GetModelLoader().GetModel(render_key.model_id);
	auto& render_resource = g_global_context.m_render_system->GetRenderResource();

	for (const auto& mesh_data : model_data.meshes)
	{
		// TODO: binding based on material
		if (model_data.textures.size() > 0)
		{
			context->PSSetShaderResources(0, 1, model_data.textures[mesh_data.m_material_index].m_diffuse.GetAddressOf());
		}
		SetupVertexBuffer(context, mesh_data, input_layout_type);
		context->IASetIndexBuffer(
			mesh_data.m_indices.Get(),
			mesh_data.m_index_count > 65535 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
		// context->DrawIndexed(static_cast<UINT>(mesh_data.m_index_count), 0, 0);
		int draw_count = std::ceil(num_models / static_cast<float>(MAX_INSTANCE_PER_DRAW_CALL));
		unsigned int instance_start_index{ 0 };
		for (int i = 0; i < draw_count; i++, instance_start_index += MAX_INSTANCE_PER_DRAW_CALL)
		{
			unsigned int instance_count = std::min(num_models - instance_start_index, MAX_INSTANCE_PER_DRAW_CALL);
			// update instance buffer
			D3D11_MAPPED_SUBRESOURCE msr{};

			context->Map(render_resource.m_buffer_instancing.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			memcpy(
				msr.pData,
				instance_payload.data() + instance_start_index,
				sizeof(BufferPerMesh) * instance_count
			);
			context->Unmap(render_resource.m_buffer_instancing.Get(), 0);
			context->VSSetShaderResources(0, 1, render_resource.m_buffer_instancing_srv.GetAddressOf());
			// draw
			context->DrawIndexedInstanced(
				mesh_data.m_index_count,
				instance_count,
				0,
				0,
				0
			);
		}
	}

	auto finish = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "updating instance data" << duration.count() << " " << std::endl;
}

ModelLoader& VertexCommon::GetModelLoader()
{
	return g_global_context.m_render_system->GetRenderResource().GetModelLoader();
}

void VertexCommon::SetupVertexBuffer(
	ID3D11DeviceContext* context, const MeshData& mesh_data, Shader::InputLayoutType input_layout_type)
{
	switch (input_layout_type)
	{
	case Shader::InputLayoutType::MESH_STATIC:
	case Shader::InputLayoutType::POS_NORMAL_TEX_TAN_COLOR:
	{
		ID3D11Buffer* pVBs[] = {
			mesh_data.m_positions.Get(),
			mesh_data.m_pTexcoordArrays.size() > 0 ? mesh_data.m_pTexcoordArrays[0].Get() : nullptr,
			mesh_data.m_normals.Get(),
			mesh_data.m_tangents.Get(),
			mesh_data.m_colors.Get()
		};
		uint32_t strides[] = { 12, 8, 12, 12, 16 };
		uint32_t offsets[] = { 0, 0, 0, 0, 0 };
		context->IASetVertexBuffers(0, ARRAYSIZE(pVBs), pVBs, strides, offsets);
		break;
	}
	default:
	{
		// not supported
		assert(false);
		break;
	}
	}
}

void VertexCommon::SetupVertexBuffer(ID3D11DeviceContext* context,
	const MeshData& mesh_data, const MeshDataSkinned mesh_data_skinned, Shader::InputLayoutType input_layout_type)
{
	switch (input_layout_type)
	{
	case Shader::InputLayoutType::MESH_SKINNED:
	{
		ID3D11Buffer* pVBs[] = {
			mesh_data.m_positions.Get(),
			mesh_data.m_pTexcoordArrays.size() > 0 ? mesh_data.m_pTexcoordArrays[0].Get() : nullptr,
			mesh_data.m_normals.Get(),
			mesh_data.m_tangents.Get(),
			mesh_data.m_colors.Get(),
			mesh_data_skinned.bone_ids.Get(),
			mesh_data_skinned.bone_weights.Get()
		};
		uint32_t strides[] = { 12, 8, 12, 12, 16, 16, 16 };
		uint32_t offsets[] = { 0, 0, 0, 0, 0, 0, 0 };
		context->IASetVertexBuffers(0, ARRAYSIZE(pVBs), pVBs, strides, offsets);
		break;
	}
	default:
	{
		// not supported
		assert(false);
		break;
	}
	}
}
