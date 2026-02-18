#include "game_object.h"
#include <cassert>
#include "global_context.h"
#include "scene/scene_manager.h"
#include "config/preset_manager.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "physics/physics_system.h"
#include "platform/timer.h"

void GameObject::AddObject(std::shared_ptr<GameObject> object)
{
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	assert(!!scene, "Failed to get current scene");
	GameObjectList& objects = scene->GetObjectList();
	objects.Add(object);
}

PresetManager& GameObject::GetPresetManager() const
{
	return *g_global_context.m_preset_manager;
}

TextureLoader& GameObject::GetTextureLoader() const
{
	return g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
}

ModelLoader& GameObject::GetModelLoader() const
{
	return g_global_context.m_render_system->GetRenderResource().GetModelLoader();
}

MaterialResourceManager& GameObject::GetMaterialResource() const
{
	return g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
}

const PhysicsScene& GameObject::GetPhysicsScene() const
{
	return g_global_context.m_physics_system->GetPhysicsScene();
}

float GameObject::GetDeltaTime() const
{
	return g_global_context.m_timer->GetDeltaTime();
}

void GameObject::FinalizeCommon()
{
	m_components.Finalize();
}

void GameObject::InitializeCommon()
{
	m_components.Initialize(shared_from_this());
}

// update layer ====
void GameObject::AddUpdateLayer(UpdateLayer layer)
{
	m_update_mask |= 1 << static_cast<int>(layer);
}

void GameObject::RemoveUpdateLayer(UpdateLayer layer)
{
	m_update_mask &= ~(1 << static_cast<int>(layer));
}

bool GameObject::IfUpdateLayer(UpdateLayer layer) const
{
	return m_update_mask & (1 << static_cast<int>(layer));
}

void GameObject::SetSceneUpdateLayer(UpdateLayer layer)
{
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	assert(!!scene, "Failed to get current scene");
	GameObjectList& objects = scene->GetObjectList();
	objects.SetUpdateLayer(layer);
}

UpdateLayer GameObject::GetSceneUpdateLayer() const
{
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	assert(!!scene, "Failed to get current scene");
	GameObjectList& objects = scene->GetObjectList();
	return objects.GetUpdateLayer();
}
