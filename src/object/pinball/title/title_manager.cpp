#include "title_manager.h"

#include "global_context.h"
#include "config/constant.h"
#include "config/camera_names.h"
#include "platform/sound.h"
#include "platform/keyboard.h"
#include "platform/controller.h"
#include "config/audio_manager.h"
#include "scene/scene_manager.h"
#include "render/resource/texture_loader.h"

#include "object/game_object_list.h"
#include "object/pinball/title/camera_title.h"
#include "object/pinball/title/player_title.h"
#include "object/pinball/title/floor_title.h"
#include "object/pinball/title/screen_main_ui_title.h"
#include "object/pinball/camera_monitor.h"
#include "object/pinball/screen_square.h"
#include "object/pinball/screen/camera_screen_main.h"
//#include "object/pinball/room_floor_visual.h"
#include "object/ui/button_enter.h"
#include "object/ui/background.h"
#include "object/common/camera_reflection_plane.h"
#include "object/common/projector.h"
#include "object/common/point_light.h"

#include "object/common/window_controller.h"

void TitleManager::Initialize()
{
	// fullscreen controller
	GetOwner().CreateGameObject<WindowController>();

	// main camera start ========
	auto camera = GetOwner().CreateGameObject<CameraTitle>();
	camera->GetTransform().SetPosition({ 0.0f, 4.5f, -16.0f });
	camera->SetTarget({ 0.0f, 3.0f, 0.0f });
	m_camera_main = camera;
	// main camera end ========

	// objects start ========
	// player
	auto player = GetOwner().CreateGameObject<PlayerTitle>();
	player->GetTransform().SetRotationYOnly(Math::PI);
	// objects end ========

	// floor start ========
	// floor object
	auto floor = GetOwner().CreateGameObject<FloorTitle>();
	floor->GetTransform().SetScale({ 40.0f, 1.0f, 40.0f });
	//  plane reflection camera
	auto camera_reflection_plane = GetOwner().CreateGameObject<CameraReflectionPlane>();
	camera_reflection_plane->SetReference(
		g_camera_presets.reflect_plane,
		camera,
		Vector3{ 0.0f, 0.0f, 0.0f },
		Vector3{ 0.0f, 1.0f, 0.0f }
	);
	// floor end ========

	// background screen start ========
	// screen
	auto& texture_loader = GetTextureLoader();
	const auto& screen_camera_preset = g_camera_presets.screen_main;
	const auto screen_texture_id = texture_loader.GetOrCreateRenderTextureId(screen_camera_preset.name);
	const Vector2 screen_pixel_scale = Vector2{
		(float)screen_camera_preset.width, (float)screen_camera_preset.height } * 0.15f;
	auto screen = GetOwner().CreateGameObject<ScreenSquare>();
	const float screen_width{ 25.0f };
	const float screen_height = screen_width * (float)screen_camera_preset.height / (float)screen_camera_preset.width;
	screen->GetTransform().SetPosition({ 0.0f, screen_height * 0.5f, 10.0f });
	screen->InitializeConfig(screen_texture_id, screen_pixel_scale, screen_width, screen_height, 0, 1);
	m_screen = screen;

	// camera monitor for background
	auto camera_monitor = GetOwner().CreateGameObject<CameraMonitor>();
	camera_monitor->GetTransform().SetPosition({ 0.0f, 3.0f, -8.0f });
	camera_monitor->SetTarget({ 0.0f, 1.6f, 0.0f });
	//CameraShapeConfig shape_config{};
	//shape_config.aspect_ratio = screen_width / screen_height;
	//shape_config.fov = 0.2f;
	//camera_monitor->SetCameraShapeConfig(shape_config);

	// camera screen ui for background
	auto camera_screen_main = GetOwner().CreateGameObject<CameraScreenMain>();

	// screen ui for background
	auto screen_main_ui = GetOwner().CreateGameObject<ScreenMainUITitle>();
	m_screen_ui = screen_main_ui;
	// background screen end ========

	// ui start ========
	auto button_enter = GetOwner().CreateGameObject<ButtonEnter>();
	button_enter->GetTransform2D().SetPosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f + 200.0f });
	m_button_enter = button_enter;
	// ui end ========

	// bgm setup start ========
	int bgm = g_global_context.m_audio_manager->LoadBgm(BgmName::BGM);
	g_global_context.m_sound->PlaySoundLoop(bgm, -1);
	// bgm setup end ========
}

void TitleManager::Update()
{
	switch (m_state)
	{
	case TitleState::IDLE:
	{
		UpdateIdle();
		break;
	}
	case TitleState::EASE_OUT:
	{
		UpdateEaseOut();
		break;
	}
	}
	// editor updates ===
	auto camera = m_camera_main.lock();
	camera->SetCameraShapeConfig(camera->GetCameraShapeConfig());
}

void TitleManager::GetEditorItem(std::vector<EditorItem>& items)
{
	{
		auto& camera_shape_config = m_camera_main.lock()->GetCameraShapeConfig();
		EditorItem camera_item{};
		camera_item.label = "Title Camera";
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
		items.push_back(camera_item);
	}
	//{
	//	auto& light = m_point_light.lock();
	//}
}

void TitleManager::UpdateIdle()
{
	if (g_global_context.m_keyboard->IsKeyTrigger(KK_ENTER))
	{
		EnterEaseOut();
		return;
	}
}

void TitleManager::EnterEaseOut()
{
	m_state_countdown.Initialize(m_config.ease_out_delay);
	m_button_enter.lock()->EnterPressed();
	m_screen.lock()->Crush();
	m_screen_ui.lock()->Crush();
	m_state = TitleState::EASE_OUT;
}

void TitleManager::UpdateEaseOut()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_GAME);
	}
}