#pragma once
#include "render/pass/pass_base_geometry.h"

class SubPassForwardUnlit : public PassBaseGeometry
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	bool ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const override;
	void SetInfoPerDraw() override;
	void SetInfoPerModel(const ModelRenderInfo& model_info) override;
private:
	void SetInfoPerMaterial(const ModelRenderKey& key);
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps{};
};