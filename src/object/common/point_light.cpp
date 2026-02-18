#include "point_light.h"
#include "component/render/component_renderer_mesh.h"
#include "component/render/component_light.h"

#include "config/preset_manager.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/resource/model_loader.h"

void PointLight::Initialize()
{
	// render
	auto& render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render);

	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");
	
	MaterialDesc material{};
	TechniqueDescDefault material_default{};
	material_default.emission_color = Vector3{ 5.0f, 0.0f, 0.0f };
	material_default.emission_intensity = 2.0f;
	material.SetTechnique(material_default);
	Model model{ model_desc, material, &m_transform };
	// model.m_model_desc.material_desc.default_desc.shading_model = 1;
	model.SetActive(false);
	render_mesh.AddModel(model);

	// light
	auto& light = m_components.Add<ComponentLight>(m_comp_id_light);
	light.SetActive(false);
}

void PointLight::SetColor(const Vector3& color)
{
	auto& light = m_components.Get<ComponentLight>(m_comp_id_light);
	light.SetColor(color * 10.0f);

	auto& model = m_components.Get<ComponentRendererMesh>(m_comp_id_render).GetModel(0);
	// TODO!
	auto& material = model.GetMaterialDesc().GetTechnique<TechniqueDescDefault>();
	material.emission_color = color;
}

void PointLight::SetLightActive(bool active)
{
	auto& light = m_components.Get<ComponentLight>(m_comp_id_light);
	light.SetActive(active);

	auto& model = m_components.Get<ComponentRendererMesh>(m_comp_id_render).GetModel(0);
	model.SetActive(active);
}

