#pragma once
#include "pass_base.h"

class PassParticleCloth : public PassBase
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;
private:
	void SetInfoPerDraw();
	void DrawItem(const class ParticleEmitter& item);
	void ResetPerDraw();

	Shader::VertexShaderInputLayout m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb{ nullptr };

	//// quad buffers for instancing
	//Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb_quad{};
	//Microsoft::WRL::ComPtr<ID3D11Buffer> m_ib_quad{};
};