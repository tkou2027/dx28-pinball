#include "object_component_manager.h"
#include "object/game_object.h"
#include "object/game_object_list.h"

void ObjectComponentManager::Initialize(std::weak_ptr<class GameObject> object)
{
	m_object = object;
}

void ObjectComponentManager::Finalize()
{
    auto& component_manager = GetComponentManager();
    for (const auto& id : m_component_ids)
    {
        component_manager.Remove(id);
    }
    m_component_ids.clear();
}

ComponentManager& ObjectComponentManager::GetComponentManager() const
{
    auto& obj_manager = m_object.lock()->GetOwner();
    return obj_manager.GetComponentManager();
}

