#pragma once
#include <memory>
#include "render/pass/pass_base_geometry.h"
#include "render/pass/vertex_common.h"

class PassDepth : public PassBaseGeometry
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	bool ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const override;
private:
	void SetInfoPerDraw() override;
	void SetInfoPerModel(const ModelRenderInfo& model_info) override;
	void SetInfoPerMaterial(const ModelRenderKey& model_info) override;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader{};
};