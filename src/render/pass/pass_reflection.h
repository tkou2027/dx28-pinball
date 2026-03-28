#pragma once
#include "render/pass/pass_base_geometry.h"

class PassReflection : public PassBaseGeometry
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	bool ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const override;
	void SetInfoPerDraw() override;
	void SetInfoPerModel(const ModelRenderInfo& model_info) override;
	void SetInputResource(
		ID3D11ShaderResourceView* buffer_normal,
		ID3D11ShaderResourceView* buffer_material,
		ID3D11ShaderResourceView* buffer_depth
	);
private:
	void SetInfoPerMaterial(const ModelRenderKey& key) override;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps{};
	// input
	ID3D11ShaderResourceView* m_buffer_normal{ nullptr };
	ID3D11ShaderResourceView* m_buffer_material{ nullptr };
	ID3D11ShaderResourceView* m_buffer_depth{ nullptr };
};