#pragma once
#include "object/game_object.h"
#include "math/camera_math.h"
#include "render/config/texture_resource_id.h"

class Projector : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(
		TextureResourceId projector_texture_id,
		const CameraShapeConfig& shape_config,
		const Vector3& position,
		const Vector3& target,
		const Vector3& up
	);
	void UpdateShape(
		const CameraShapeConfig& shape_config,
		const Vector3& position,
		const Vector3& target,
		const Vector3& up
	);
	void UpdateTexture(TextureResourceId projector_texture_id);
private:
	ComponentId m_comp_id_mesh{};
};