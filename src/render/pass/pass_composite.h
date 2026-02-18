#pragma once
#include "pass_base.h"

class PassComposite : public PassBase
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Draw() override;
	//void SetInputTextureBase(ID3D11ShaderResourceView* srv);
	//void SetInputTextureReflection(ID3D11ShaderResourceView* srv);
	//void SetInputTextureEmmision(ID3D11ShaderResourceView* srv);
	void DrawAdd(ID3D11ShaderResourceView* srv_a, ID3D11ShaderResourceView* srv_b);
	void DrawAddReflection(
		ID3D11ShaderResourceView* srv_base,
		ID3D11ShaderResourceView* srv_reflection,
		ID3D11ShaderResourceView* srv_g_buffer_material
	);
	void DrawToneMapping(ID3D11ShaderResourceView* srv);
private:
	Shader::VertexShaderInputLayout m_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_add;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_add_reflection;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_tone_mapping;
};