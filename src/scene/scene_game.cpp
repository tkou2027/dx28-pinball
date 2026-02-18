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

	// auto test = m_objects.CreateGameObject<TestEffect>();

	//auto screen = m_objects.CreateGameObject<Screen>();
	//auto floor = m_objects.CreateGameObject<TestFloor>();

	//auto enemy_center = m_objects.CreateGameObject<EnemyCenter>();
	//enemy_center->InitializeConfig({});
	
	Hex hex{};
	hex.SetSize(2.8f);
	//for (int k = 0; k < 2; k++)
	//{

		for (int r = 30; r <= 34; r++)
		{
			std::vector<HexCoord> coords = HexCoord::GenerateRing(r);
			for (int i = 0; i < coords.size(); ++i)
			{
				float pos_x = hex.HexToX(coords[i]);
				float pos_z = hex.HexToZ(coords[i]);
				float pos_y = (r - 20.0f) * 1.5f;
				auto effect = m_objects.CreateGameObject<TestTexture>();
				effect->GetTransform().SetPosition({ pos_x, pos_y, pos_z });
				effect->GetTransform().SetScale(0.15f);

				float rotation_y = std::atan2(-pos_x, -pos_z);
				float dist_xz = std::sqrt(pos_x * pos_x + pos_z * pos_z);
				float rotation_x = std::atan2(dist_xz, -pos_y);
				//effect->GetTransform().SetRotationEuler({ rotation_x, rotation_y, 0.0f });
				effect->GetTransform().SetRotationYOnly(rotation_y);
			}
		}
	//}
		
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
