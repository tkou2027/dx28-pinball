#pragma once
#include "render/pass/pass_base.h"
#include "render/resource/render_attachment.h"

class PassSSR : public PassBase
{
public:
	struct InternalTextures
	{
		TextureResource2D texture_reflection_info;
		TextureResource2DReadWrite texture_reflection_color_raw;
		TextureResource2D texture_reflection_color; // mip map
		TextureResource2DReadWrite texture_hi_z;
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
	void DrawHiZ();
	void DrawUVTexture();
	void DrawRefine();
	void DrawBlur();

	Shader::VertexShaderInputLayout m_vs{};
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_hi_z_copy{};
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_hi_z_gen{};
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_refine{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_uv{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_blur{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_hi_z{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_ssr{};

	// input textures
	static constexpr int BUFFER_NUM{ 4 };
	ID3D11ShaderResourceView* m_srv_g_buffer[BUFFER_NUM]{};
	ID3D11ShaderResourceView* m_srv_color_texture{ nullptr };
	const InternalTextures* m_textures{ nullptr };
};