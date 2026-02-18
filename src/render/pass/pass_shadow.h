#pragma once
#include <array>

#include "pass_base.h"

#include <DirectXMath.h>

class PassShadow : public PassBase
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;
	void SetMatrix(DirectX::XMMATRIX world);
	void Begin();
	void End();
private:
	enum VertexShaderIndex : size_t
	{
		VS_INDEX_MESH_STATIC = 0,
		VS_INDEX_MESH_SKINNED = 1,
		VS_INDEX_MAX = 2
	};
	void SetInfoPerDraw();
	void SetInfoPerShader(VertexShaderIndex vertex_shader_index);
	void SetInfoPerModel(const ModelRenderInfo& entity);

	std::array<Shader::VertexShaderInputLayout, VS_INDEX_MAX> m_vertex_shaders;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer_per_shadow{ nullptr };
};