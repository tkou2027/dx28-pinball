#include "shader.h"

#include <fstream>
#include <vector>
#include "util/debug_ostream.h"

using namespace Microsoft::WRL;

// input layouts
namespace
{
	static const std::wstring SHADER_ROOT_PATH{ L"resource/shader_build/" };

	static const D3D11_INPUT_ELEMENT_DESC INPUT_DESC_MESH_STATIC[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC INPUT_DESC_POS[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC INPUT_DESC_POS_TEX_NORMAL_TANGENT_COLOR[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC INPUT_DESC_MESH_SKINNED[]
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT,  5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC INPUT_DESC_APPEND_POS_TEX[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static const D3D11_INPUT_ELEMENT_DESC INPUT_DESC_APPEND_POS_TEX_COLOR[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	bool LoadShaderFile(const std::wstring& cso_file, std::vector<unsigned char>& file_data)
	{
		// 事前コンパイル済み頂点シェーダーの読み込み
		std::ifstream ifs(SHADER_ROOT_PATH + cso_file, std::ios::binary);

		if (!ifs)
		{
			MessageBox(nullptr, "シェーダーの読み込みに失敗しました", "エラー", MB_OK);
			return false;
		}

		// ファイルサイズを取得
		ifs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
		std::streamsize filesize = ifs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
		ifs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

		// バイナリデータを格納するためのバッファを確保
		// unsigned char* gsbinary_pointer = new unsigned char[filesize];
		file_data.resize(filesize);
		ifs.read(reinterpret_cast<char*>(file_data.data()), filesize); // バイナリデータを読み込む
		ifs.close(); // ファイルを閉じる
		return true;
	}
}

Shader::VertexShaderInputLayout Shader::CreateShaderVertex(ID3D11Device* device, const std::wstring& cso_file, InputLayoutType layout_type)
{
	VertexShaderInputLayout vertex_shader_input_layout{};
	CreateShaderVertex(device, cso_file, layout_type,
		vertex_shader_input_layout.vertex_shader, vertex_shader_input_layout.input_layout);
	vertex_shader_input_layout.layout_type = layout_type;
	return vertex_shader_input_layout;
}

void Shader::CreateShaderVertex(ID3D11Device* device, const std::wstring& cso_file, InputLayoutType layout_type,
	ComPtr<ID3D11VertexShader>& vertex_shader, ComPtr<ID3D11InputLayout>& input_layout)
{
	std::vector<unsigned char> file_data;
	LoadShaderFile(cso_file, file_data);

	// 頂点シェーダーの作成
	HRESULT hr = device->CreateVertexShader(file_data.data(), file_data.size(), nullptr, vertex_shader.GetAddressOf());

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
		return;
	}

	// 頂点レイアウトの定義
	const D3D11_INPUT_ELEMENT_DESC* layout;
	UINT num_elements{}; // 配列の要素数を取得
	GetInputLayoutDesc(layout_type, layout, num_elements);

	// 頂点レイアウトの作成
	hr = device->CreateInputLayout(layout, num_elements, file_data.data(), file_data.size(), input_layout.GetAddressOf());
}

ComPtr<ID3D11PixelShader> Shader::CreateShaderPixel(ID3D11Device* device, const std::wstring& cso_file)
{
	std::vector<unsigned char> file_data;
	LoadShaderFile(cso_file, file_data);

	ComPtr<ID3D11PixelShader> pixel_shader;
	HRESULT hr = device->CreatePixelShader(file_data.data(), file_data.size(), nullptr, pixel_shader.GetAddressOf());

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : ピクセルシェーダーの作成に失敗しました" << std::endl;
		return nullptr;
	}
	return pixel_shader;
}

ComPtr<ID3D11GeometryShader> Shader::CreateShaderGeometry(ID3D11Device* device, const std::wstring& cso_file)
{
	std::vector<unsigned char> file_data;
	LoadShaderFile(cso_file, file_data);

	ComPtr<ID3D11GeometryShader> geo_shader;
	HRESULT hr = device->CreateGeometryShader(file_data.data(), file_data.size(), nullptr, geo_shader.GetAddressOf());

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : Geometryシェーダーの作成に失敗しました" << std::endl;
		return nullptr;
	}
	return geo_shader;
}

ComPtr<ID3D11ComputeShader> Shader::CreateShaderCompute(ID3D11Device* device, const std::wstring& cso_file)
{
	std::vector<unsigned char> file_data;
	LoadShaderFile(cso_file, file_data);

	ComPtr<ID3D11ComputeShader> compute_shader;
	HRESULT hr = device->CreateComputeShader(file_data.data(), file_data.size(), nullptr, compute_shader.GetAddressOf());

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : Computeシェーダーの作成に失敗しました" << std::endl;
		return nullptr;
	}
	return compute_shader;
}

void Shader::GetInputLayoutDesc(InputLayoutType type, const D3D11_INPUT_ELEMENT_DESC*& layout, UINT& num_elements)
{
	switch (type)
	{
	case InputLayoutType::MESH_STATIC:
	{
		//layout = INPUT_DESC_MESH_STATIC;
		//num_elements = ARRAYSIZE(INPUT_DESC_MESH_STATIC);
		//break;
		layout = INPUT_DESC_POS_TEX_NORMAL_TANGENT_COLOR;
		num_elements = ARRAYSIZE(INPUT_DESC_POS_TEX_NORMAL_TANGENT_COLOR);
		break;
	}
	case InputLayoutType::MESH_SKINNED:
	{
		layout = INPUT_DESC_MESH_SKINNED;
		num_elements = ARRAYSIZE(INPUT_DESC_MESH_SKINNED);
		break;
	}
	case InputLayoutType::SPRITE_DEFAULT:
	{
		layout = INPUT_DESC_APPEND_POS_TEX_COLOR;
		num_elements = ARRAYSIZE(INPUT_DESC_APPEND_POS_TEX_COLOR);
		break;
	}
	case InputLayoutType::APPEND_POS_TEX:
	{
		layout = INPUT_DESC_APPEND_POS_TEX;
		num_elements = ARRAYSIZE(INPUT_DESC_APPEND_POS_TEX);
		break;
	}
	//case InputLayoutType::POS_NORMAL:
	//{
	//	layout = INPUT_DESC_MESH_POS_MORMAL;
	//	num_elements = ARRAYSIZE(INPUT_DESC_MESH_POS_MORMAL);
	//	break;
	//}
	case InputLayoutType::POS:
	{
		layout = INPUT_DESC_POS;
		num_elements = ARRAYSIZE(INPUT_DESC_POS);
		break;
	}
	case InputLayoutType::POS_NORMAL_TEX_TAN_COLOR:
	{
		layout = INPUT_DESC_POS_TEX_NORMAL_TANGENT_COLOR;
		num_elements = ARRAYSIZE(INPUT_DESC_POS_TEX_NORMAL_TANGENT_COLOR);
		break;
	}
	case InputLayoutType::NONE:
	{
		layout = nullptr;
		num_elements = 0;
		break;
	}
	}
}