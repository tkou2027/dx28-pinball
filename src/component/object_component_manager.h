#pragma once
#include <vector>
#include <memory>
#include <typeindex>
#include "component/component.h"
#include "component/component_manager.h"

class ObjectComponentManager
{
public:
	void Initialize(std::weak_ptr<class GameObject> game_object);
	void Finalize(); // remove all components on object delete
	// add component
	template<typename TComponent>
	TComponent& Add(ComponentId& id) // out id
	{
		auto& component_manager = GetComponentManager();
		id = component_manager.Add<TComponent>(m_object);
		m_component_ids.emplace_back(id);
		return component_manager.GetComponent<TComponent>(id);
	}
	template<typename TComponent>
	TComponent& Add() // no out id
	{
		ComponentId id{};
		return Add<TComponent>(id);
	}
	// get component (internal)
	template<typename TComponent>
	TComponent& Get(ComponentId id)
	{
		auto& component_manager = GetComponentManager();
		return component_manager.GetComponent<TComponent>(id);
	}
	template<typename TComponent>
	const TComponent& Get(ComponentId id) const
	{
		const auto& component_manager = GetComponentManager();
		return component_manager.GetComponent<TComponent>(id);
	}
	// get first component of type if exists
	 template<typename TComponent>
	 ComponentId TryGetComponent() const
	 {
		 for (ComponentId id : m_component_ids)
		 {
			 const auto& component_manager = GetComponentManager();
			 const auto& comp_info = component_manager.GetComponentInfo(id);
			 if (comp_info.type_id == typeid(TComponent))
			 {
				 return id;
			 }
		 }
		 return -1;
	 }
private:
	class ComponentManager& GetComponentManager() const;
	std::weak_ptr<class GameObject> m_object;
	std::vector<ComponentId> m_component_ids;
};