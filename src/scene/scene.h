#pragma once
#include "object/game_object_list.h"

class Scene
{
public:
	virtual ~Scene() = default;
	virtual void Initialize()
	{
		m_objects.Initialize();
	}
	virtual void Update()
	{
		m_objects.Update();
	};
	virtual void Finalize()
	{
		m_objects.Finalize();
	};
	void OnInitialized()
	{
		m_objects.OnSceneInitialized();
	}
	GameObjectList& GetObjectList()
	{
		return m_objects;
	}
	const std::list<std::shared_ptr<GameObject> >& GetObjects() const
	{
		return m_objects.GetList();
	}
protected:
	GameObjectList m_objects;
};