#include "title_manager.h"

#include "global_context.h"
#include "config/constant.h"
#include "platform/sound.h"
#include "platform/keyboard.h"
#include "platform/controller.h"
#include "config/audio_manager.h"
#include "scene/scene_manager.h"

#include "object/game_object_list.h"
#include "object/pinball/title/camera_title.h"
#include "object/ui/button_enter.h"
#include "object/ui/background.h"

void TitleManager::Initialize()
{
	auto camera = GetOwner().CreateGameObject<CameraTitle>();

	auto background = GetOwner().CreateGameObject<Background>();
	background->SetSprite("sprite/ui/bg_title");

	auto button_enter = GetOwner().CreateGameObject<ButtonEnter>();
	button_enter->GetTransform2D().SetPosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f + 100.0f });
	m_button_enter = button_enter;

	// bgm
	int bgm = g_global_context.m_audio_manager->LoadBgm(BgmName::BGM);
	g_global_context.m_sound->PlaySoundLoop(bgm, -1);
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