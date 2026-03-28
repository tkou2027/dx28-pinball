#include "floor_title.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "config/camera_names.h"

#include "render/resource/texture_loader.h"

void FloorTitle::Initialize()
{
	auto& mesh_comp = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);

	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane");
	auto& texture_loader = GetTextureLoader();

	MaterialDesc material{};
	TechniqueDescDefault material_default{};
	material_default.metallic = 1.0f;
	material_default.roughness = 0.1f;
	material_default.specular = 0.5f; // reflection mask
	material_default.base_color = Vector3{ 0.0f, 0.0f, 0.0f };
	material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_NormalDX.jpg");
	material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_Metalness.jpg");
	//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_Roughness.jpg");
	// TODO: texture
	material.SetTechnique(material_default);

	const auto reflection_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.reflect_plane.name);
	TechniqueDescForwardReflectionPlane material_reflection;
	material_reflection.texture_id = reflection_texture_id;
	material.SetTechnique(material_reflection);

	Model model{ model_desc, material, &m_transform };
	model.GetUVAnimationState().uv_scroll_size = Vector2{ 1.0f, 1.0f } * 40.0f;

	mesh_comp.AddModel(model);
}
