#pragma once

#include "object/game_object.h"
#include "component/render/component_renderer_mesh.h"
#include "global_context.h"
#include "config/preset_manager.h"

#include "shader_setting.h"

class TestModelObject : public GameObject
{
public:
	void Initialize() override
	{
		m_components.Add<ComponentRendererMesh>(m_comp_id_render);
		
	}
	void Config(std::string model_preset_key, MaterialDesc material_desc)
	{
		const auto& model_desc = g_global_context.m_preset_manager->GetModelDesc(model_preset_key);
		Model state{ model_desc, material_desc, &m_transform };
		auto& render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
		render_mesh.AddModel(state);
	}
	void Config(ModelDesc model_desc, MaterialDesc material_desc)
	{
		Model state{ model_desc, material_desc, &m_transform};
		auto& render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
		render_mesh.AddModel(state);
	}
	void Update() override
	{
		auto& model = m_components.Get<ComponentRendererMesh>(m_comp_id_render).GetModel(0);
		//if (model.m_model_desc.material_desc.default_desc.shading_model == 2)
		//{
		//	model.m_model_desc.material_desc.default_desc.metallic = g_shader_setting.cel_offset / 6.0f;
		//}
	}
private:
	ComponentId m_comp_id_render;
};