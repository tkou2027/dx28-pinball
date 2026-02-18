#pragma once
#include <memory>
#include "pass_base_geometry.h"
#include "vertex_common.h"
#include "render/pass/geometry/subpass_geometry_default.h"

class PassGeometry : public PassBase
{
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Draw() override;
	void AddSubPass(std::unique_ptr<PassBaseGeometry> subpass);
	void ResetRenderableIndices(CameraRenderLayer layer);
	void AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc);
private:
	std::vector <std::unique_ptr<PassBaseGeometry> > m_variations{};
	size_t m_default_pass_index{ 0 };
};