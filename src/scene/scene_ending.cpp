#include "scene_ending.h"
#include "object/pinball/title/ending_manager.h"

void SceneEnding::Initialize()
{
	m_objects.CreateGameObject<EndingManager>();
}