#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cassert>
#include "component/component.h"
#include "component/component_array.h"

class GameObject;
typedef size_t ComponentId;
class ComponentManager
{
public:
	struct ComponentInfo
	{
		std::type_index type_id;
		size_t array_index;
	};
	void Initialize();
	void Update();
	void Finalize();
	// add component
	template<typename TComponent>
	ComponentId Add(std::weak_ptr<GameObject> object);
	// delete
	void Remove(ComponentId id);
	// get components of type
	template<typename TComponent>
	std::vector<TComponent>& GetComponents();
	// get component
	template<typename TComponent>
	TComponent& GetComponent(ComponentId id);
	// get component
	template<typename TComponent>
	const TComponent& GetComponent(ComponentId id) const;
	// find info utils
	const ComponentInfo& GetComponentInfo(ComponentId id) const;
private:
	// find array utils
	template<typename TComponent>
	ComponentArray<TComponent>& GetOrCreateArray();
	template<typename TComponent>
	ComponentArray<TComponent>& GetArray() const;
	IComponentArray& GetArray(std::type_index) const;
	// find info utils
	ComponentInfo& GetComponentInfo(ComponentId id);
	std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> m_type_to_component_array;
	// managing index
	ComponentId m_next_id{ 0 };
	std::unordered_map<ComponentId, ComponentInfo> m_id_to_info;
};

#include "component_manager.inl"