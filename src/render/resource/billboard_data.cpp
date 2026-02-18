#include "billboard_data.h"
#include <algorithm>
#include "render/dx_trace.h"

void BillboardData::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	InitializeBuffer(INITIAL_CAPACITY);
}

void BillboardData::ClearData()
{
	m_instances_sort.clear();
}

void BillboardData::AddInstances(const BillboardDesc& desc, const std::vector<BillboardInstance>& instances)
{
	for (const auto& inst : instances)
	{
		if (!inst.GetActive()) continue;

		BillboardInstanceSortable entry{};
		entry.texture_id = desc.texture_id;

		//DirectX::XMVECTOR pos = inst.GetPosition().ToXMVECTOR();
		//DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(pos, camear_pos);
		//DirectX::XMStoreFloat(&entry.depth, DirectX::XMVector3LengthSq(diff));

		entry.data.position = inst.GetPosition().ToXMFLOAT3();
		entry.data.size = inst.GetSize().ToXMFLOAT2();
		const auto& uv_rect = inst.GetUV(desc);
		entry.data.uv_offset = uv_rect.uv_offset.ToXMFLOAT2();
		entry.data.uv_size = uv_rect.uv_size.ToXMFLOAT2();
		// entry.data.rotation = 0.0f;
		// entry.data.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_instances_sort.push_back(entry);
	}
}

void BillboardData::UpdateData()
{
	m_instances_flat.clear();
	m_instances_flat.reserve(m_instances_sort.size());
	m_commands.clear();

	if (m_instances_sort.empty())
	{
		return;
	}

	std::sort(m_instances_sort.begin(), m_instances_sort.end(),
		[](const auto& a, const auto& b)
		{
			//// first sort by depth
			//if (!Math::IsZero(a.depth - b.depth))
			//{
			//	return a.depth_sq < -1.0f || a.depth_sq > b.depth_sq; // no sorting -> far -> near
			//}
			return a.texture_id < b.texture_id;
		}
	);

	BillboardBatchCommand current_cmd = { m_instances_sort[0].texture_id, 0, 0 };
	for (uint32_t i = 0; i < m_instances_sort.size(); ++i) {
		const auto& inst = m_instances_sort[i];
		if (inst.texture_id != current_cmd.texture_id) {
			m_commands.push_back(current_cmd);
			current_cmd.texture_id = inst.texture_id;
			current_cmd.start_index = i;
			current_cmd.instance_count = 0;
		}
		m_instances_flat.push_back(inst.data);
		current_cmd.instance_count++;
	}
	m_commands.push_back(current_cmd);

	UpdateDataBuffer();
}

void BillboardData::InitializeBuffer(uint32_t capacity)
{
	m_capacity = capacity;

	// structured buffer
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(BillboardInstanceData) * m_capacity;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(BillboardInstanceData);
	HR(m_device->CreateBuffer(&desc, nullptr, m_buffer.ReleaseAndGetAddressOf()));

	// srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_capacity;

	HR(m_device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_srv.ReleaseAndGetAddressOf()));
}

void BillboardData::UpdateDataBuffer()
{
	if (m_instances_flat.empty())
	{
		return;
	}

	// no resize because i'm lazy...
	assert(m_instances_flat.size() <= m_capacity);

	// update buffer
	D3D11_MAPPED_SUBRESOURCE msr;
	HR(m_context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr));
	const auto data_size = sizeof(BillboardInstanceData) * m_instances_flat.size();
	memcpy(msr.pData, m_instances_flat.data(), data_size);
	m_context->Unmap(m_buffer.Get(), 0);
}
