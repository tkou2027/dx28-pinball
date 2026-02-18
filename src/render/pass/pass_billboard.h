#pragma once
#include "pass_base.h"
#include <wrl/client.h>

class PassBillboard : public PassBase
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Draw() override;

private:
	void SetInfoPerDraw();
	Shader::VertexShaderInputLayout m_vs_layout{};
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_gs{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps{};
};