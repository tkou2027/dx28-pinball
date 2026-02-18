#include "scene_title.h"
#include "object/pinball/title/title_manager.h"

void SceneTitle::Initialize()
{
	m_objects.CreateGameObject<TitleManager>();
}
