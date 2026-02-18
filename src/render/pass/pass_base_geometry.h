#pragma once
#include "pass_base.h"
#include "vertex_common.h"
#include "render/resource/view_context.h"

class PassBaseGeometry : public PassBase
{
public:
	virtual ~PassBaseGeometry() = default;
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context,
		VertexCommon::VertexCommonConfig config);
	void Draw() override;

	virtual void ResetRenderableIndices(CameraRenderLayer layer);
	virtual bool AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc);
	virtual bool ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const = 0;
protected:
	// TODO: shaders should be saved as resources
	VertexCommon m_vertex_common{};

	virtual void SetInfoPerDraw();
	virtual bool SetInfoPerVertexShader(ModelType type);
	virtual void SetInfoPerModel(const ModelRenderInfo& model_info);
	virtual void SetInfoPerMaterial(const ModelRenderKey& model_info) {};

	std::vector<size_t> m_mesh_indices_static;
	std::vector<size_t> m_mesh_indices_skinned;
	std::vector<size_t> m_mesh_indices_instanced;
	CameraRenderLayer m_curr_render_layer{ CameraRenderLayer::DEFAULT };
};