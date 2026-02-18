#pragma once
#include "render/directx.h"

class Buffer
{
public:
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateConstantBuffer(
		ID3D11Device* device, size_t size);
	static void CreateStructuredBuffer(
		ID3D11Device* device, size_t stride, size_t elements,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv
	);
	// with uav (for gpu particles)
	static void CreateStructuredBuffer(
		ID3D11Device* device, size_t stride, size_t elements,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav
	);
};