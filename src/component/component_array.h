#pragma once
#include <vector>
#include <memory>
#include "component.h"

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void Update() = 0;
    virtual void Finalize() = 0;
	virtual ComponentId GetComponentIdAt(int index) = 0;
	virtual int RemoveAt(int index) = 0;
};

template<typename TComponent>
class ComponentArray : public IComponentArray
{
	static_assert(
		std::is_base_of_v<Component, TComponent>,
		"TComponent must derive from Component"
	);
public:
	void Update() override;
    void Finalize() override;
	ComponentId GetComponentIdAt(int index);
	int RemoveAt(int index) override;
	// component
	int AddComponent(ComponentId id, std::weak_ptr<class GameObject> object);
	std::vector<TComponent>& GetComponents();
	TComponent& GetComponent(int index);
	void RemoveComponent(int index);
private:
	std::vector<TComponent> m_components; // value for continuous memory 
};


template<typename TComponent>
void ComponentArray<TComponent>::Update()
{
    for (auto& comp : m_components)
    {
        if (comp.GetActive())
        {
            comp.Update();
        }
    }
}

template<typename TComponent>
void ComponentArray<TComponent>::Finalize()
{
    for (auto& comp : m_components)
    {
        comp.Finalize();
    }
}

template<typename TComponent>
ComponentId ComponentArray<TComponent>::GetComponentIdAt(int index)
{
    auto& comp = m_components.at(index);
    return comp.GetId();
}

template<typename TComponent>
int ComponentArray<TComponent>::RemoveAt(int index)
{
    m_components[index].Finalize();
    int last = static_cast<int>(m_components.size()) - 1;
    if (index != last)
    {
        // swap with last element
        m_components[index] = std::move(m_components[last]);
        m_components.pop_back();
        return last; // return swapped index
    }
    m_components.pop_back();
    return -1; // no swap
}

template<typename TComponent>
int ComponentArray<TComponent>::AddComponent(ComponentId id, std::weak_ptr<GameObject> object)
{
    m_components.emplace_back();
    // set owner
    auto& comp = m_components.back();
    comp.SetId(id);
    comp.SetOwner(object);
    int index = static_cast<int>(m_components.size() - 1);
    return index;
}

template<typename TComponent>
std::vector<TComponent>& ComponentArray<TComponent>::GetComponents()
{
    return m_components;
}

template<typename TComponent>
TComponent& ComponentArray<TComponent>::GetComponent(int index)
{
    return m_components.at(index);
}

template<typename TComponent>
void ComponentArray<TComponent>::RemoveComponent(int index)
{
    // TODO
    m_components.erase(m_components.begin() + index);
}
