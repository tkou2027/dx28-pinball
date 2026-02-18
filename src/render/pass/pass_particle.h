#pragma once
#include "pass_base.h"

#include <DirectXMath.h>

class PassParticle: public PassBase
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;
private:
	void SetInfoPerDraw();
	void DrawItem(const class TextureParticleItem& item);
	void ResetPerDraw();

	Shader::VertexShaderInputLayout m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;

	// quad buffers for instancing
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb_quad{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ib_quad{};
};