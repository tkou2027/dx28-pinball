#pragma once

#include <vector>
#include <memory>
#include "component/object_component_manager.h"
#include "update_layer.h"
#include "math/transform2d.h"
#include "math/transform3d.h"
#include "editor/editor_item.h"

class GameObject : public std::enable_shared_from_this<GameObject>
{
	friend class GameObjectList;
public:
	template<typename TGameObject>
	static std::shared_ptr<TGameObject> Create()
	{
		auto obj = std::make_shared<TGameObject>();
		obj->Initialize();
		return obj;
	}
	virtual ~GameObject() = default;
	virtual void Initialize() {};
	virtual void Update() {};
	virtual void Draw() {};
	virtual void Finalize() {};
	virtual void OnSceneInitialized() {};
	virtual void OnSceneStart() {};
	void SetObjectActive(bool active) { m_object_active = active; }
	bool GetObjectActive() const { return m_object_active; }

	// component
	TransformNode3D& GetTransform() { return m_transform; }
	const TransformNode3D& GetTransform() const { return m_transform; }
	TransformNode2D& GetTransform2D() { return m_transform_2d; }
	const TransformNode2D& GetTransform2D() const { return m_transform_2d; }
	const ObjectComponentManager& GetComponents() const { return m_components; }
	// update layer
	void AddUpdateLayer(UpdateLayer layer);
	void RemoveUpdateLayer(UpdateLayer layer);
	bool IfUpdateLayer(UpdateLayer layer) const;
	void SetSceneUpdateLayer(UpdateLayer layer);
	UpdateLayer GetSceneUpdateLayer() const;
	// collision
	virtual void OnCollision(const struct CollisionInfo& collision) {}
	virtual void OnTrigger(const struct CollisionInfo& collision) {}
	// editor
	virtual void GetEditorItem(std::vector<EditorItem>& items) {}
	// object list
	void SetOwner(GameObjectList* owner) { m_owner = owner; }
	GameObjectList& GetOwner() { return *m_owner; }
	void AddObject(std::shared_ptr<GameObject> object);
	void FinalizeCommon();
protected:
	// global resource
	class PresetManager& GetPresetManager() const;
	class TextureLoader& GetTextureLoader() const;
	class ModelLoader& GetModelLoader() const;
	class MaterialResourceManager& GetMaterialResource() const;
	const class PhysicsScene& GetPhysicsScene() const;
	float GetDeltaTime() const;

	GameObject() = default;
	bool m_object_active{ true };
	ObjectComponentManager m_components;
	TransformNode3D m_transform{};
	TransformNode2D m_transform_2d{};
	int m_update_mask{ 1 << static_cast<int>(UpdateLayer::DEFAULT) };
private:
	void InitializeCommon();

	GameObjectList* m_owner{ nullptr };
};

