#pragma once

#include <vector>
#include <list>
#include <memory>
#include "game_object.h"
#include "update_layer.h"
#include "component/component_manager.h"

class GameObjectList
{
public:
	static constexpr int MAX_OBJECTS{ 
		256 };
	void Initialize();
	void Update();
	void Finalize();
	void OnSceneInitialized();
	void Add(std::shared_ptr<GameObject> object);
	const std::list<std::shared_ptr<GameObject> >& GetList() const { return m_objects; }
	void SetUpdateLayer(UpdateLayer layer) { m_update_layer = layer; }
	UpdateLayer GetUpdateLayer() const { return m_update_layer; }
	// create
	template<typename TGameObject>
	std::shared_ptr<TGameObject> CreateGameObject()
	{
		auto obj = std::make_shared<TGameObject>();
		obj->InitializeCommon();
		obj->SetOwner(this);
		Add(obj);
		obj->Initialize();
		return obj;
	}
	// get
	template<class T>
	std::shared_ptr<T> FindGameObject()
	{
		for (auto& game_object : m_objects)
		{
			if (auto casted = std::dynamic_pointer_cast<T>(game_object))
			{
				return casted;
			}
		}
		return nullptr;
	}
	template<class T>
	std::vector<std::shared_ptr<T> >FindGameObjects()
	{
		std::vector<std::shared_ptr<T> > res;
		for (auto& game_object : m_objects)
		{
			if (auto casted = std::dynamic_pointer_cast<T>(game_object))
			{
				res.push_back(casted);
			}
		}
		return res;
	}
	ComponentManager& GetComponentManager()
	{
		return m_component_manager;
	}
private:
	void HandleRemove();
	std::list<std::shared_ptr<GameObject> > m_objects{};
	UpdateLayer m_update_layer{ UpdateLayer::DEFAULT };
	ComponentManager m_component_manager;
};
