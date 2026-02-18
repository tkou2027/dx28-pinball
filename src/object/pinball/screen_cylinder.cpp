#include "screen_cylinder.h"
#include "component/render/component_renderer_mesh.h"

void ScreenCylinder::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
}

void ScreenCylinder::InitializeConfig(float radius, float height)
{
	m_radius = radius;
	m_height = height;

	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_4x1");

	auto& camera_preset = g_camera_presets.screen_status;
	auto& texture_loader = GetTextureLoader();
	const auto screen_texture_id = texture_loader.GetOrCreateRenderTextureId(camera_preset.name);
	const auto pixel_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pixel_mask.png");

	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.emission_intensity = 2.0f;
	material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
	material_default.cull_type = CullType::CULL_NONE;
	material_default.shading_model = ShadingModel::UNLIT;
	material_default.albedo_texture_id = screen_texture_id;
	material_default.no_deferred = true;
	material_desc.SetTechnique(material_default, CameraRenderLayerMask::REFLECTED | CameraRenderLayerMask::DEFAULT);

	TechniqueDescForwardScreen material_screen{};
	material_screen.screen_pixel_texture_id = pixel_texture_id;
	material_screen.screen_pixels_scale = Vector2{
		static_cast<float>(camera_preset.width), static_cast<float>(camera_preset.height) } * 0.25f;
	material_desc.SetTechnique(material_screen, CameraRenderLayerMask::DEFAULT);

	TechniqueDescForwardUnlit material_unlit{};
	material_desc.SetTechnique(material_unlit, CameraRenderLayerMask::REFLECTED);

	Model model{ model_desc, material_desc, &m_transform };
	model.GetTransform().SetScale({ m_radius, m_height, m_radius });
	model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);

	comp_render_mesh.AddModel(model);
}
