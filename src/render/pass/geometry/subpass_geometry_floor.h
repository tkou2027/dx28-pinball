#pragma once
#include <array>
#include "render/pass/pass_base_geometry.h"
#include "render/pass/vertex_common.h"

class PassGeometryFloor : public PassBaseGeometry
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
private:
	bool ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const override;
	// per path constant buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_material{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_floor_config{ nullptr };


	void SetInfoPerDraw() override;
	void SetInfoPerModel(const ModelRenderInfo& model_info) override;
	void SetInfoPerMaterial(const ModelRenderKey& model_info) override;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader{};
};