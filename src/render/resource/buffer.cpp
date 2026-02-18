#include "buffer.h"
#include "render/dx_trace.h"
using namespace Microsoft::WRL;

ComPtr<ID3D11Buffer> Buffer::CreateConstantBuffer(ID3D11Device* device, size_t size)
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	ComPtr<ID3D11Buffer> buffer{ nullptr };
	HR(device->CreateBuffer(&desc, nullptr, buffer.ReleaseAndGetAddressOf()));
	return buffer;
}

void Buffer::CreateStructuredBuffer(
	ID3D11Device* device, size_t stride, size_t elements,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv
)
{
	// structured buffer
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(stride * elements);
	desc.Usage = D3D11_USAGE_DYNAMIC; // dynamic for cpu write
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = static_cast<UINT>(stride);
	HR(device->CreateBuffer(&desc, nullptr, buffer.ReleaseAndGetAddressOf()));

	// srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(elements);

	HR(device->CreateShaderResourceView(
		buffer.Get(), &srvDesc, srv.ReleaseAndGetAddressOf()));
}

void Buffer::CreateStructuredBuffer(
	ID3D11Device* device, size_t stride, size_t elements,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav)
{
	// structured buffer
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(stride * elements);
	desc.Usage = D3D11_USAGE_DEFAULT; // default for GPU write
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; // uavÅI
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = static_cast<UINT>(stride);
	HR(device->CreateBuffer(&desc, nullptr, buffer.ReleaseAndGetAddressOf()));

	// srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(elements);
	HR(device->CreateShaderResourceView(
		buffer.Get(), &srvDesc, srv.ReleaseAndGetAddressOf()));

	// uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = static_cast<UINT>(elements);
	device->CreateUnorderedAccessView(buffer.Get(), &uavDesc, uav.ReleaseAndGetAddressOf());
}
