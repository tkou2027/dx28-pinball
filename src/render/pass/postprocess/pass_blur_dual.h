#pragma once
#include "render/pass/pass_base.h"
#include "render/resource/render_attachment.h"
class PassBlurDual : public PassBase
{
public:
	static constexpr int MAX_MIP_LEVELS{ 5 };
	struct InternalTextures
	{
		uint32_t initial_width{};
		uint32_t initial_height{};
		int mip_levels{ 0 };
		TextureResource2D mip_textures[MAX_MIP_LEVELS];
	};

	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;

	void CreateTextures(uint32_t width, uint32_t height, InternalTextures& out_textures);
	void SetInputResource(const InternalTextures* textures, ID3D11ShaderResourceView* srv_input);
private:
	void SetInfoPerDraw();
	void DrawDownSample();
	void DrawUpSample();
	void UnbindResources();

	Shader::VertexShaderInputLayout m_vs{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_down{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_up{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_down{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_up{ nullptr };

	// context
	ID3D11ShaderResourceView* m_srv_input{ nullptr };
	const InternalTextures* m_textures{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps_add{};	
};