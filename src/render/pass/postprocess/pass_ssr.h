#pragma once
#include "render/pass/pass_base.h"
#include "render/resource/render_attachment.h"

class PassSSR : public PassBase
{
public:
	struct InternalTextures
	{
		TextureResource2D texture_reflection_info;
		TextureResource2D texture_reflection_color; // mip map
		TextureResource2D texture_reflection_blurred;
	};
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Draw() override;
	// set input textures
	void CreateTextures(uint32_t width, uint32_t height, InternalTextures& out_textures);
	void SetInputResource(
		const InternalTextures* textures,
		ID3D11ShaderResourceView* g_buffer_a,
		ID3D11ShaderResourceView* g_buffer_b,
		ID3D11ShaderResourceView* g_buffer_c,
		ID3D11ShaderResourceView* g_buffer_depth,
		ID3D11ShaderResourceView* color_texture
	);
private:
	void DrawUVTexture();
	void DrawBlur();

	Shader::VertexShaderInputLayout m_vs{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_uv{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_blur{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_ssr{};

	// input textures
	static constexpr int BUFFER_NUM{ 4 };
	ID3D11ShaderResourceView* m_srv_g_buffer[BUFFER_NUM]{};
	ID3D11ShaderResourceView* m_srv_color_texture{ nullptr };
	const InternalTextures* m_textures{ nullptr };
};