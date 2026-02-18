#include "component_renderer_mesh.h"
#include "object/game_object.h"

ComponentRendererMesh::ComponentRendererMesh(int num_models)
{
	m_model_states.reserve(num_models);
}

void ComponentRendererMesh::Update()
{
	for (auto& model_state : m_model_states)
	{
		model_state.Update();
	}
}

int ComponentRendererMesh::AddModel(const Model& state)
{
	int index = m_model_states.size();
	m_model_states.push_back(state);
	return index;
}

std::vector<Model>& ComponentRendererMesh::GetModels()
{
	return m_model_states;
}

Model& ComponentRendererMesh::GetModel(int index)
{
	return m_model_states[index];
}
