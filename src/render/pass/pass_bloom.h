#pragma once
#include "pass_base.h"
#include "render/resource/render_attachment.h"
class PassBloom : public PassBase
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;
	void DrawOutput();
	void SetInputTextureBlur(ID3D11ShaderResourceView* input) { m_srv_blur = input; }
	void SetInputTextureAdd(ID3D11ShaderResourceView* input) { m_srv_add = input; }
	ID3D11ShaderResourceView* GetRt() {
		return m_rt_add.GetTextureColor<TextureResource2D>(0)->GetShaderResourceView().Get();
	}
private:
	Shader::VertexShaderInputLayout m_vs{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_blur{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_add{};

	ID3D11ShaderResourceView* m_srv_blur{ nullptr };
	ID3D11ShaderResourceView* m_srv_add{ nullptr };
	RenderAttachment m_rt_blur{}; // ping pong
	RenderAttachment m_rt_add{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_blur{};
};