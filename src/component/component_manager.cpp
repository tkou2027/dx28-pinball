#include "component_manager.h"
#include <cassert>

void ComponentManager::Initialize()
{
	m_type_to_component_array.clear();
}

void ComponentManager::Update()
{
	for (auto& pair : m_type_to_component_array)
	{
		pair.second->Update();
	}
}

void ComponentManager::Finalize()
{
	for (auto& pair : m_type_to_component_array)
	{
		pair.second->Finalize();
	}
	m_type_to_component_array.clear();
}

void ComponentManager::Remove(ComponentId id)
{
	auto it = m_id_to_info.find(id);
	assert(it != m_id_to_info.end());
	auto& array = GetArray(it->second.type_id);
	const int curr_index = it->second.array_index;
	int swapped_index = array.RemoveAt(curr_index);

	// remove info
	m_id_to_info.erase(it);

	// update swapped info
	if (swapped_index < 0)
	{
		// no swap
		return;
	}
	// swa[
	ComponentId swapped_id = array.GetComponentIdAt(curr_index);
	auto& swapped_comp_info = GetComponentInfo(swapped_id);
	swapped_comp_info.array_index = curr_index; // update index
}

IComponentArray& ComponentManager::GetArray(std::type_index type_id) const
{
	auto it = m_type_to_component_array.find(type_id);
	assert(it != m_type_to_component_array.end());
	return *it->second.get();
}

ComponentManager::ComponentInfo& ComponentManager::GetComponentInfo(ComponentId id)
{
	auto it = m_id_to_info.find(id);
	assert(it != m_id_to_info.end());
	return it->second;
}

const ComponentManager::ComponentInfo& ComponentManager::GetComponentInfo(ComponentId id) const
{
	auto it = m_id_to_info.find(id);
	assert(it != m_id_to_info.end());
	return it->second;
}
