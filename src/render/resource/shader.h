#pragma once
#include <string>
#include "render/directx.h"

class Shader
{
public:
	enum class InputLayoutType
	{
		MESH_STATIC,
		MESH_SKINNED,

		POS,
		POS_NORMAL_TEX_TAN_COLOR,
		SPRITE_DEFAULT,
		APPEND_POS_TEX,
		NONE
	};
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct VertexShaderInputLayout
	{
		ComPtr<ID3D11VertexShader> vertex_shader{};
		ComPtr<ID3D11InputLayout> input_layout{};
		InputLayoutType layout_type{ InputLayoutType::NONE };
	};
	static VertexShaderInputLayout CreateShaderVertex(ID3D11Device* device, const std::wstring& cso_file, InputLayoutType layout_type);
	static void CreateShaderVertex(ID3D11Device* device, const std::wstring& cso_file, InputLayoutType layout_type,
		ComPtr<ID3D11VertexShader>& vertex_shader, ComPtr<ID3D11InputLayout>& input_layout);
	static ComPtr<ID3D11PixelShader> CreateShaderPixel(ID3D11Device* device, const std::wstring& cso_file);
	static ComPtr<ID3D11GeometryShader> CreateShaderGeometry(ID3D11Device* device, const std::wstring& cso_file);
	static ComPtr<ID3D11ComputeShader> CreateShaderCompute(ID3D11Device* device, const std::wstring& cso_file);
	static void GetInputLayoutDesc(InputLayoutType type, const D3D11_INPUT_ELEMENT_DESC*& layout, UINT& num_elements);
};