#pragma once

#pragma once
#include "component/component.h"
#include "math/vector3.h"
#include "render/config/camera_data.h"

class ComponentCamera : public Component
{
public:
	void InitializeCamera(
		const CameraUsageConfig& config, const CameraShapeConfig& shape);
	void Update() override;
	void Finalize() override;

	const CameraUsageConfig& GetUsageConfig() const { return m_usage_config; }
	const CameraShapeConfig& GetShapeConfig() const { return m_shape_config;  }
	// TODO: dirty flag
	void SetShapeConfig(const CameraShapeConfig& shape) { m_shape_config = shape; }
	// state
	void SetTarget(const Vector3& target) { m_target = target; }
	const Vector3& GetTarget() const { return m_target; }
	void SetUp(const Vector3& up) { m_up = up; }
	const Vector3& GetPosition() const { return m_position; }
	const Vector3& GetUp() const { return m_up; }
	void SetViewMatrix(const DirectX::XMMATRIX& view_matrix);

	void GetDirectionXZNormalized(Vector3& out_forward, Vector3& out_right) const;
private:
	CameraUsageConfig m_usage_config{};
	CameraShapeConfig m_shape_config{};

	Vector3 m_up{ 0.0f, 1.0f, 0.0f };
	Vector3 m_target{ 0.0f, 5.0f, 0.0f };
	Vector3 m_position{ 0.0f, 0.0f, 0.0f };

	bool m_initialized{ false };
};