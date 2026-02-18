#pragma once
#include "object/game_object.h"

#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "config/preset_manager.h"
#include "config/camera_names.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"

class ScreenCylinder : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(float radius, float height);
private:
	ComponentId m_comp_id_mesh{};

	float m_radius{};
	float m_height{};
};