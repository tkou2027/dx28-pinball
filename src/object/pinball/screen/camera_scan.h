#pragma once
#include "object/game_object.h"

class CameraScan : public GameObject
{
public:
	void Initialize() override;
	void SetCameraMain(std::weak_ptr<class GameObject> camera_main);
	void SetReference(std::weak_ptr<class CameraScanDummy> scan_dummy);
	void Update() override;
private:
	ComponentId m_comp_id_camera{};
	ComponentId m_comp_id_reference_camera_main{};
	std::weak_ptr<class CameraScanDummy> m_scan_dummy{};
	std::weak_ptr<class GameObject> m_camera_main{};
};