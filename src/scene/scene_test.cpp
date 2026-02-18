#include "scene_test.h"

#include "object/common/camera_editor.h"
#include "object/effect/test_effect.h"
#include "object/effect/test_texture.h"
#include "object/common/skybox.h"
//#include "object/pinball/camera_screen_main.h"
#include "object/pinball/screen.h"
#include "math/hex.h"

void SceneTest::Initialize()
{
	InitializeTestScreen();
	// InitializeTestTexture();
}

void SceneTest::InitializeTestScreen()
{
	auto camera = m_objects.CreateGameObject<CameraEditor>();
	auto effect = m_objects.CreateGameObject<TestEffect>();
	auto skybox = m_objects.CreateGameObject<Skybox>();

	const Vector3 center{ effect->GetTransform().GetPositionGlobal() };
	const Vector3 offset{ 0.0f, 50.0f, -400.0f };
	camera->SetFocus(effect->GetTransform().GetPositionGlobal(), offset);
}

void SceneTest::InitializeTestTexture()
{
	auto camera = m_objects.CreateGameObject<CameraEditor>();
	//auto camera_screen = m_objects.CreateGameObject<CameraScreenMain>();
	auto screen = m_objects.CreateGameObject<Screen>();
	// auto effect = m_objects.CreateGameObject<TestTexture>();

	auto skybox = m_objects.CreateGameObject<Skybox>();
	//auto effect = m_objects.CreateGameObject<TestEffect>();

	const Vector3 center{};
	const Vector3 offset{ 0.0f, 20.0f, -50.0f };
	camera->SetFocus(center, offset);

	Hex hex{};
	for (int k = 0; k < 2; k++)
	{

		for (int r = 24; r <= 32; r++)
		{
			std::vector<HexCoord> coords = HexCoord::GenerateRing(r);
			for (int i = 0; i < coords.size(); ++i)
			{
				float pos_x = hex.HexToX(coords[i]);
				float pos_z = hex.HexToZ(coords[i]);
				float pos_y = r - 20.0f;
				auto effect = m_objects.CreateGameObject<TestTexture>();
				effect->GetTransform().SetPosition({ pos_x, pos_y, pos_z });
				effect->GetTransform().SetScale(0.05f);

				float rotation_y = std::atan2(-pos_x, -pos_z);
				float dist_xz = std::sqrt(pos_x * pos_x + pos_z * pos_z);
				float rotation_x = std::atan2(dist_xz, -pos_y);
				//effect->GetTransform().SetRotationEuler({ rotation_x, rotation_y, 0.0f });
				effect->GetTransform().SetRotationYOnly(rotation_y);
			}
		}
	}

}
