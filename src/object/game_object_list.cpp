#include "game_object_list.h"
#include <cassert>

void GameObjectList::Initialize()
{
	// m_objects.reserve(MAX_OBJECTS);
	m_component_manager.Initialize();
}

void GameObjectList::Update()
{
	int index{ 0 };
	for (auto& obj : m_objects)
	{
		assert(obj);
		if (obj->GetObjectActive() && obj->IfUpdateLayer(m_update_layer))
		{
			obj->Update();
		}
		index++;
	}
	HandleRemove();
	m_component_manager.Update();
}

void GameObjectList::Finalize()
{
	for (auto& obj : m_objects)
	{
		obj->FinalizeCommon();
		obj->Finalize();
	}
	m_component_manager.Finalize();
	m_objects.clear();
}

void GameObjectList::OnSceneInitialized()
{
	for (auto& obj : m_objects)
	{
		obj->OnSceneInitialized();
	}
	for (auto& obj : m_objects)
	{
		obj->OnSceneStart();
	}
}

void GameObjectList::Add(std::shared_ptr<GameObject> object)
{
	assert(object, "Adding null object");
	object->SetOwner(this);
	m_objects.push_back(object);
}

void GameObjectList::HandleRemove()
{
	std::list<std::shared_ptr<GameObject>> remaining;
	for (auto& obj : m_objects)
	{
		if (obj->GetObjectActive())
		{
			remaining.push_back(obj);
		}
		else
		{
			obj->FinalizeCommon();
			obj->Finalize();
		}
	}
	m_objects.swap(remaining);
}
