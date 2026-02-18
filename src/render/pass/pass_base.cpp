#include "pass_base.h"

#include "render/directx.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/dx_trace.h"
#include "config/constant.h" // TODO
#include "render/config/model_state.h"

using namespace Microsoft::WRL;

void PassBase::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
}

const RenderScene& PassBase::GetRenderScene() const
{
	return g_global_context.m_render_system->GetRenderScene();
}

const RenderResource& PassBase::GetRenderResource() const
{
	return g_global_context.m_render_system->GetRenderResource();
}

const RenderStates& PassBase::GetRenderStates() const
{
	return g_global_context.m_render_system->GetRenderStates();
}

TextureLoader& PassBase::GetTextureLoader()
{
	return g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
}

const TextureLoader& PassBase::GetTextureLoader() const
{
	return g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
}

ModelLoader& PassBase::GetModelLoader()
{
	return g_global_context.m_render_system->GetRenderResource().GetModelLoader();
}

const MaterialResourceManager& PassBase::GetMaterialResourceManager() const
{
	return g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
}

int PassBase::GetScreenWidth() const
{
	return SCREEN_WIDTH;
}

int PassBase::GetScreenHeight() const
{
	return SCREEN_HEIGHT;
}

ComPtr<ID3D11Buffer> PassBase::CreateConstantBuffer(size_t size) const
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	ComPtr<ID3D11Buffer> buffer;
	HR(m_device->CreateBuffer(&desc, nullptr, buffer.ReleaseAndGetAddressOf()));
	return buffer;
}

void PassBase::DrawModelStatic(const ModelRenderInfo& model)
{
	const auto& model_data = GetModelLoader().GetModel(model.key.model_id);
	for (const auto& mesh_data : model_data.meshes)
	{
		if (model_data.textures.size() > 0)
		{
			m_context->PSSetShaderResources(0, 1, model_data.textures[mesh_data.m_material_index].m_diffuse.GetAddressOf());
		}

		// environment map
		//auto env_texture_id = model.model_desc.material_desc.default_desc.env_texture_id;
		//if (env_texture_id >= 0)
		//{
		//	const auto& texture_loader = GetTextureLoader();
		//	auto srv = texture_loader.GetTexture(env_texture_id);
		//	m_context->PSSetShaderResources(1, 1, srv.GetAddressOf());
		//}

		if (mesh_data.m_pTexcoordArrays.size() > 0)
		{
			ID3D11Buffer* pVBs[] = { mesh_data.m_positions.Get(), mesh_data.m_normals.Get(),
				mesh_data.m_pTexcoordArrays[0].Get() };
			uint32_t strides[] = { 12, 12, 8 };
			uint32_t offsets[] = { 0, 0, 0 };
			m_context->IASetVertexBuffers(0, ARRAYSIZE(pVBs), pVBs, strides, offsets);
		}
		else
		{
			ID3D11Buffer* pVBs[] = { mesh_data.m_positions.Get(), mesh_data.m_normals.Get() };
			uint32_t strides[] = { 12, 12 };
			uint32_t offsets[] = { 0, 0 };
			m_context->IASetVertexBuffers(0, ARRAYSIZE(pVBs), pVBs, strides, offsets);
		}
		m_context->IASetIndexBuffer(
			mesh_data.m_indices.Get(),
			mesh_data.m_index_count > 65535 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_context->DrawIndexed(static_cast<UINT>(mesh_data.m_index_count), 0, 0);
	}
}

void PassBase::DrawModelDynamic(const ModelRenderInfo& model)
{
	const auto& model_data = GetModelLoader().GetModel(model.key.model_id);
	const auto& model_data_skinned = GetModelLoader().GetModelSkinned(model.key.model_skinning_id);
	const auto& model_data_skinned_matrix = GetModelLoader().GetModelSkinnedMatrix(model.key.model_skinning_matrix_id);
	//m_context->UpdateSubresource(
	//	model_data_dynamic.bone_matrix_buffer.Get(), 0, nullptr, model.bones_matrix.data(), 0, 0);
	//D3D11_MAPPED_SUBRESOURCE msr;
	//m_context->Map(model_data_skinned.bone_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	//memcpy(msr.pData, model.bones_matrix.data(), sizeof(DirectX::XMFLOAT4X4) * model.bones_matrix.size());
	//m_context->Unmap(model_data_skinned.bone_matrix_buffer.Get(), 0);

	// set bone matrix srv
	m_context->VSSetShaderResources(0, 1, model_data_skinned_matrix.bone_matrix_srv.GetAddressOf());
	int index{ 0 };
	for (const auto& mesh_data : model_data.meshes)
	{
		m_context->PSSetShaderResources(0, 1, model_data.textures[mesh_data.m_material_index].m_diffuse.GetAddressOf());

		ID3D11Buffer* pVBs[] = { mesh_data.m_positions.Get(), mesh_data.m_normals.Get(),
			mesh_data.m_pTexcoordArrays.size() > 0 ? mesh_data.m_pTexcoordArrays[0].Get() : nullptr,
			 model_data_skinned.meshes_skinned[index].bone_ids.Get(),  model_data_skinned.meshes_skinned[index].bone_weights.Get() };
		uint32_t strides[] = { 12, 12, 8, 16, 16 };
		uint32_t offsets[] = { 0, 0, 0, 0, 0 };
		m_context->IASetVertexBuffers(0, ARRAYSIZE(pVBs), pVBs, strides, offsets);
		m_context->IASetIndexBuffer(
			mesh_data.m_indices.Get(),
			mesh_data.m_index_count > 65535 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
		m_context->DrawIndexed(static_cast<UINT>(mesh_data.m_index_count), 0, 0);
		index++;
	}
}
