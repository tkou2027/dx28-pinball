#pragma once
#include <memory>
#include "component_layer.h"

typedef size_t ComponentId;
class GameObject;
class Component
{
public:
	virtual ~Component() = default;
	virtual void Update() {};
	virtual void Finalize() {};
	// state
	virtual bool GetActive() const { return m_active; }
	virtual void SetActive(bool active) { m_active = active; }
	// layer
	ComponentLayer GetLayer() const { return m_layer; }
	void SetLayer(ComponentLayer layer) { m_layer = layer; }
	// object
	void SetOwner(std::weak_ptr<GameObject> object) { m_object = object; }
	std::weak_ptr<GameObject> GetOwner() const { return m_object; }
	// id
	ComponentId GetId() const { return m_id; }
	void SetId(ComponentId id) { m_id = id; }
protected:
	bool m_active{ true };
	ComponentLayer m_layer{ ComponentLayer::DEFAULT };
	std::weak_ptr<GameObject> m_object{};
	// GameObject* m_object{};
	ComponentId m_id{};
};