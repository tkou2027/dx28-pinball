#include "game_manager.h"

#include "object/game_object_list.h"
#include "object/common/skybox.h"

#include "object/pinball/field.h"
#include "object/pinball/player.h"
#include "object/pinball/bumper_manager.h"
#include "object/pinball/camera_follow.h"
#include "object/pinball/camera_monitor.h"
#include "object/pinball/screen/camera_screen_main.h"
#include "object/pinball/screen/camera_screen_status.h"
#include "object/pinball/camera_reflect.h"

// system
#include "object/common/hit_stop_updater.h"
#include "object/common/window_controller.h"
//#include "object/pinball/test_billboard.h"

#include "global_context.h"
#include "scene/scene_manager.h"

#include "config/audio_manager.h"
#include "platform/sound.h"

void GameManager::Initialize()
{

	// main camera
	auto camera = GetOwner().CreateGameObject<CameraFollow>();
	m_camera_main = camera;
	// auto camera_monitor = GetOwner().CreateGameObject<CameraMonitor>();
	auto camera_screen = GetOwner().CreateGameObject<CameraScreenMain>();
	auto camera_screen_status = GetOwner().CreateGameObject<CameraScreenStatus>();
	auto camera_reflect = GetOwner().CreateGameObject<CameraReflect>();
	// auto screen_status_ui = GetOwner().CreateGameObject<ScreenStatusUI>();
	auto bumper_manager = GetOwner().CreateGameObject<BumperManager>();
	bumper_manager->InitializeConfig(g_field_config);

	auto field = GetOwner().CreateGameObject<Field>();
	field->InitializeConfig(g_field_config);
	m_field = field;

	auto player = GetOwner().CreateGameObject<Player>();

	const auto& room_config = g_field_config.rooms.at(0);
	player->GetTransform().SetPosition(
		room_config.player_active_position);

	camera->SetTargetObject(player, { 0.0f, 20.0f, 0.0f });
	camera->SetFollowCentered(
		room_config.camera_center, { 0.0f, 36.0f, 100.0f });

	// follow player
	auto skybox = GetOwner().CreateGameObject<Skybox>();
	GetOwner().CreateGameObject<HitStopUpdater>();
	GetOwner().CreateGameObject<WindowController>();

	// bgm
	int bgm = g_global_context.m_audio_manager->LoadBgm(BgmName::BGM);
	g_global_context.m_sound->PlaySoundLoop(bgm, -1);
}

void GameManager::OnSceneStart()
{
	auto field = m_field.lock();
	field->StartFade();
	field->SetTransferActiveRoom(0);
}

void GameManager::Update()
{
	auto field = m_field.lock();
	if (field->IfDone())
	{
		g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_ENDING);
	}

	// editor updates ===
	auto camera = m_camera_main.lock();
	camera->SetCameraShapeConfig(camera->GetCameraShapeConfig());
}


void GameManager::GetEditorItem(std::vector<EditorItem>& items)
{
	// camera
	{
		auto& camera_shape_config = m_camera_main.lock()->GetCameraShapeConfig();

		EditorItem camera_item{};
		camera_item.label = "Game Camera";
		// fov
		{
			EditorProperty prop{};
			prop.label = "FOV";
			prop.data_ptr = &camera_shape_config.fov;
			prop.type = EditorPropertyType::FLOAT;
			prop.min = 0.1f;
			prop.max = 3.0f;
			camera_item.properties.push_back(prop);
		}
		// follow distance
		{
			EditorProperty prop{};
			prop.label = "Follow CenterOffset";
			prop.data_ptr = &m_camera_main.lock()->GetTargetPositionOffset().x;
			prop.type = EditorPropertyType::FLOAT3;
			prop.min = -200.0f;
			prop.max = 200.0f;
			camera_item.properties.push_back(prop);
		}
		// follow distance
		{
			EditorProperty prop{};
			prop.label = "Follow Offset";
			prop.data_ptr = &m_camera_main.lock()->GetFollowOffset().x;
			prop.type = EditorPropertyType::FLOAT3;
			prop.min = -100.0f;
			prop.max = 100.0f;
			camera_item.properties.push_back(prop);
		}

		items.push_back(camera_item);
	}
}
