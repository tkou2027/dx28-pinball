#pragma once
#include "pass_base.h"
#include "pass_base_geometry.h"

class PassDeferredShading : public PassBase
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Draw() override;
	void SetGBuffer(const class RenderAttachment& geo_buffer);
	void SetTextureShadow(ID3D11ShaderResourceView* srv);
	void SetInputResource(
		ID3D11ShaderResourceView* g_buffer_a,
		ID3D11ShaderResourceView* g_buffer_b,
		ID3D11ShaderResourceView* g_buffer_c,
		ID3D11ShaderResourceView* g_buffer_depth
	);
	void SetIBLTextures(
		const std::string& diffuse,
		const std::string& specular,
		const std::string& brdf_lut
	);
private:
	void DrawLocalLights();

	Shader::VertexShaderInputLayout m_vs{};
	Shader::VertexShaderInputLayout m_vs_local_light{};

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_debug{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_local_light_shading{};

	// Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_blur{};
	static constexpr int GEO_BUFFER_NUM{ 4 };
	ID3D11ShaderResourceView* m_srv_g_buffer[GEO_BUFFER_NUM]{};
	ID3D11ShaderResourceView* m_srv_shadow;

	TextureResourceId m_texture_ibl_diffuse{};
	TextureResourceId m_texture_ibl_specular{};
	TextureResourceId m_texture_ibl_brdf_lut{};
};
