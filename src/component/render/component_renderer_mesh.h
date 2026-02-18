#pragma once
#include <string>
#include <vector>
#include "component/component.h"
#include "render/config/model_desc.h"
#include "render/config/model_state.h"
#include "render/config/material_desc.h"

class ComponentRendererMesh : public Component
{
public:
	ComponentRendererMesh(int num_sprites = 1);
	void Update() override;
	int AddModel(const Model& state);
	std::vector<Model>& GetModels();
	Model& GetModel(int index);
private:
	// std::vector<ModelDesc> m_models;
	std::vector<Model> m_model_states;
	std::vector<ModelRenderInfo> m_render_entities;
};