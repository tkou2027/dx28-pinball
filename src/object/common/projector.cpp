#include "projector.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"

using namespace DirectX;

void Projector::Initialize()
{
	auto& comp_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");
	MaterialDesc material{};
	TechniqueDescForwardProjector material_projector{};
	material.SetTechnique(material_projector);
	Model model{ model_desc, material, &m_transform };
	comp_mesh.AddModel(model);
	comp_mesh.SetActive(false);
}

void Projector::InitializeConfig(
	TextureResourceId projector_texture_id,
	const CameraShapeConfig& shape_config,
	const Vector3& position, const Vector3& target, const Vector3& up)
{
	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	// comp_mesh.SetActive(true); // TODO
	UpdateTexture(projector_texture_id);
	UpdateShape(shape_config, position, target, up);
}

void Projector::UpdateShape(
	const CameraShapeConfig& shape_config,
	const Vector3& position, const Vector3& target, const Vector3& up)
{
	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);

	// model transform
	m_transform.SetPosition(position);

	// projector params
	auto& material_desc = comp_mesh.GetModel(0).GetMaterialDesc();
	auto& material_desc_projector = material_desc.GetTechnique<TechniqueDescForwardProjector>();
	material_desc_projector.projector_position_w = position;

	// compute view projection matrix
	XMMATRIX mat_view = CameraMath::CalculateViewMatrix(position, target, up);
	XMMATRIX mat_proj = CameraMath::CalculateProjectionMatrix(shape_config);
	XMMATRIX mat_view_proj = mat_view * mat_proj;
	XMStoreFloat4x4(&material_desc_projector.projector_mat_view_proj, XMMatrixTranspose(mat_view_proj));
}

void Projector::UpdateTexture(TextureResourceId projector_texture_id)
{
	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& material_desc = comp_mesh.GetModel(0).GetMaterialDesc();
	auto& material_desc_projector = material_desc.GetTechnique<TechniqueDescForwardProjector>();
	material_desc_projector.projectior_texture_id = projector_texture_id;
}
