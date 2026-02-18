#pragma once

#include "pass_base.h"

class PassPostprocess : public PassBase
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Draw() override;
	void SetInputResource(ID3D11ShaderResourceView* srv);
private:
	Shader::VertexShaderInputLayout m_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;
	ID3D11ShaderResourceView* m_srv_input{ nullptr };
};