#pragma once
#include "pass_base.h"
#include "render/config/sprite_desc.h"
class PassSprite : public PassBase
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;
	void SetRenderLayer(CameraRenderLayer layer);
private:
	void Begin();
	void DrawSprite(const SpriteOutput& sprite);

	Shader::VertexShaderInputLayout m_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constant_buffer;

	CameraRenderLayer m_curr_render_layer{ CameraRenderLayer::DEFAULT };
};