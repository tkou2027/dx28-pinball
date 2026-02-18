#pragma once
#include "pass_base.h"
#include <DirectXMath.h>

class PassSky : public PassBase
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Draw() override;
	void ResetRenderableIndices(CameraRenderLayer layer);
	void AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc);
private:
	void SetInfoPerDraw();
	void SetInfoPerModel(const ModelRenderInfo& model);
	void SetMatrix(DirectX::XMMATRIX world);
	Shader::VertexShaderInputLayout m_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;

	std::vector<size_t> m_mesh_indices_static;
	CameraRenderLayer m_camera_render_layer{ CameraRenderLayer::DEFAULT };
};
