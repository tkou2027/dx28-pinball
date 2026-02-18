#pragma once
#include <vector>
#include <DirectXMath.h>
#include "render/config/billboard.h"
#include "render/config/texture_resource_id.h"
#include "render/directx.h"

// gpu
struct BillboardInstanceData
{
	DirectX::XMFLOAT3 position;
	float rotation{ 0 }; // padding
	DirectX::XMFLOAT2 size;
	DirectX::XMFLOAT2 uv_offset;
	DirectX::XMFLOAT2 uv_size;
	// TODO: color ?
};

struct BillboardInstanceSortable
{
	TextureResourceId texture_id;
	float depth{ -1.0f };
	BillboardInstanceData data;
};

struct BillboardBatchCommand {
	TextureResourceId texture_id;
	uint32_t start_index;
	uint32_t instance_count;
};

class BillboardData
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	void ClearData();
	void AddInstances(const BillboardDesc& desc, const std::vector<BillboardInstance>& instances);
	void UpdateData();

	const std::vector<BillboardBatchCommand>& GetBatchCommands() const { return m_commands; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
private:
	static constexpr uint32_t INITIAL_CAPACITY{ 2048 };
	void InitializeBuffer(uint32_t capacity);
	void UpdateDataBuffer();
	std::vector<BillboardInstanceSortable> m_instances_sort; // data for cpu
	std::vector<BillboardBatchCommand> m_commands; // draw state / command for gpu
	std::vector<BillboardInstanceData> m_instances_flat; // data for gpu
	// gpu data
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

	uint32_t m_capacity{ 0 };

	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};