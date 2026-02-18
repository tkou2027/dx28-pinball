#pragma once
#include <d3d11.h>
#include <DirectXMath.h> // TODO replace with matrix
#include "object/game_object.h"
#include "math/vector2.h"
#include "math/vector3.h"
class Camera : public GameObject
{
public:
	struct CameraConfig
	{
		Vector3 up{ 0.0f, 1.0f, 0.0f };
		Vector3 target{ 0.0f, 5.0f, 0.0f };
		Vector3 from{};
		float z_near{ 1.0f };
		float z_far{ 800.0f };
		float fov{ 0.5f };
		Vector2 screen_size{};
	};
	void Initialize();
	void Finalize() {};
	void Update();
	void SetConfig(const CameraConfig& config);
	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;
private:
	float GetTargetRotation() const;
	void SetTargetRotation(float radiant);
	void SetTargetZoom(float delta_dist);
	float GetTargetRotationVertical() const;
	void SetTargetRotationVertical(float radiant);
	void MoveTargetScreenX(float delta_dist);
	void MoveTargetScreenY(float delta_dist);
	CameraConfig m_config{};
};