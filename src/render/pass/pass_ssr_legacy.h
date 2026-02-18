#pragma once
#include "pass_base.h"
#include "render/resource/render_attachment.h"
#include "pass_deferred_shading.h"
class PassSSRLegacy : public PassBase
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;
	// set input textures
	void SetInputResource(
		ID3D11ShaderResourceView* g_buffer_a,
		ID3D11ShaderResourceView* g_buffer_b,
		ID3D11ShaderResourceView* g_buffer_c,
		ID3D11ShaderResourceView* g_buffer_depth,
		ID3D11ShaderResourceView* color_texture
	);

	void SetGBuffer(const class RenderAttachment& geo_buffer);
	void SetColorTexture(ID3D11ShaderResourceView* srv);
	ID3D11ShaderResourceView* GetRt() {
		return m_rt_blur.GetTextureColor<TextureResource2D>(0)->GetShaderResourceView().Get(); }
private:
	void DrawUVTexture();
	void DrawBlur();
	void DrawBlend();

	Shader::VertexShaderInputLayout m_vs{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_uv{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_blur{};

	static constexpr int BUFFER_NUM{ 4 };
	ID3D11ShaderResourceView* m_srv_g_buffer[BUFFER_NUM]{};
	ID3D11ShaderResourceView* m_srv_color_texture{ nullptr };

	RenderAttachment m_rt_reflection{};
	RenderAttachment m_rt_blur{};

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_ssr{};
};