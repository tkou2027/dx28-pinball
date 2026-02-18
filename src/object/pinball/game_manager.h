#pragma once
#include "object/game_object.h"

class GameManager : public GameObject
{
public:
	void Initialize() override;
	void OnSceneStart() override;
	void Update() override;
	void GetEditorItem(std::vector<EditorItem>& items) override;
private:
	std::weak_ptr<class Field> m_field{};
	std::weak_ptr<class CameraFollow> m_camera_main{};
};