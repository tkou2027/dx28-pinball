#pragma once
#include "scene.h"

class SceneManager
{
public:
	enum class SceneName
	{
		SCENE_NONE,
		SCENE_TITLE,
		SCENE_GAME,
		SCENE_ENDING,

		SCENE_TEST
	};
	void Initialize();
	void Finalize();
	void Update();
	Scene* GetCurrentScene() { return m_scene_current; };
	void SetNextScene(SceneName next);
private:
	void UpdateTransfer();

	SceneName m_scene_name_current{ SceneName::SCENE_NONE };
	SceneName m_scene_name_next{ SceneName::SCENE_NONE };
	Scene* m_scene_current;
};