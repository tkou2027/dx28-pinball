#pragma once
#include "render/pass/pass_base.h"
class PassBlurDual : public PassBase
{
public:
	enum class Mode
	{
		NORMAL,
		MATERIAL,
		DEPTH,
		SRGB
	};
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void DrawMode(Mode mode, ID3D11ShaderResourceView* srv_in);
private:
	Shader::VertexShaderInputLayout m_vs{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_up{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb{ nullptr };
};