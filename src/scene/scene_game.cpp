#include "scene_game.h"
#include "object/pinball/game_manager.h"

//#include "object/pinball/camera_monitor.h"
//#include "object/pinball/camera_screen_main.h"
//#include "object/pinball/camera_reflect.h"
#include "object/pinball/screen.h"

#include "object/pinball/enemy_center.h"

#include "math/hex.h"
#include "object/effect/test_texture.h"
#include "object/effect/test_floor.h"
#include "object/effect/test_effect.h"
#include "object/common/point_light.h"


void SceneGame::Initialize()
{
	m_objects.CreateGameObject<GameManager>();

	//for (int i = -11; i < 11; i++)
	//{
	//	auto light = m_objects.CreateGameObject<PointLight>();
	//	light->GetTransform().SetPosition({ i * 10.0f, 1.0f, 0.0f });
	//
	//	light->SetColor({ 1.0f, 1.0f, 0.5f });
	//	light->SetLightActive(true);
	//}
	//for (int i = -11; i < 11; i++)
	//{
	//	auto light = m_objects.CreateGameObject<PointLight>();
	//	light->GetTransform().SetPosition({ 0.0f, 1.0f, i * 10.0f });
	//
	//	light->SetColor({ 1.0f, 1.0f, 0.5f });
	//	light->SetLightActive(true);
	//}
}
