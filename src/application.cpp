#include "application.h"

#include "global_context.h"
#include "platform/timer.h"
#include "platform/window.h"
#include "platform/keyboard.h"
#include "platform/controller.h"
#include "render/render_system.h"
#include "physics/physics_system.h"
#include "scene/scene_manager.h"
#include "config/preset_manager.h"
#include "editor/editor.h"
#include "editor/editor_ui.h"

void Application::Initialize(const WindowConfig& window_config)
{
	g_global_context.StartSystems(window_config);
	g_global_context.m_preset_manager->LoadPresets();
	g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_GAME);

	g_global_context.m_window->Show();
}

int Application::Run()
{
// #ifdef _DEBUG
	int count_fps{ 0 };
	auto time_last_tick{ g_global_context.m_timer->GetTimePoint() };
	auto time_curr_tick{ time_last_tick };
// #endif
	do {
		if (g_global_context.m_window->HandleMessage())
		{
			continue;
		}
		if (!g_global_context.m_timer->CheckUpdateNextFrame()) // 60fps
		{
			continue;
		}

// #ifdef _DEBUG
			// 1秒ごとに実行
			float time_since_tick = g_global_context.m_timer->GetTimeSinceTimePoint(time_last_tick, time_curr_tick);
			if (time_since_tick >= 1.0f)
			{
				// デバッグ版の時だけFPSを表示する
				g_global_context.m_window->SetDebugTitle(count_fps / time_since_tick);
				time_last_tick = time_curr_tick; // FPSを測定した時刻を保存
				count_fps = 0; // カウントをクリア
			}
// #endif

		// ゲーム処理
		Update();// 更新処理			
		Draw();  // 描画処理

// #ifdef _DEBUG
		++count_fps; // 処理回数のカウントを加算
// #endif
	} while (!g_global_context.m_window->ShouldQuit());

	return g_global_context.m_window->GetQuitResult();
}

void Application::Finalize()
{
	g_global_context.ShutdownSystems();
}

void Application::Update()
{
	//auto start = std::chrono::high_resolution_clock::now();

	//g_global_context.m_sound->Update();
	//g_global_context.m_controller->Update();
	g_global_context.m_controller->Update();

	// imgui ----
#ifdef _DEBUG
	g_global_context.m_editor->GetEditorUI().Update();
#endif
	//// game update
	g_global_context.m_scene_manager->Update(); // object scripts
	g_global_context.m_physics_system->Update(); // solve collisions

	//auto finish = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	//FOO_LOG("upadte took %d ms\n", duration.count());
	g_global_context.m_keyboard->Update(); // 前の状態を更新、最後に呼び出す

}

void Application::Draw()
{
	//auto start = std::chrono::high_resolution_clock::now();
	//g_global_context.m_render_system->ClearBuffers();
	// g_global_context.m_graphics->Clear();
	g_global_context.m_render_system->Draw();
#ifdef _DEBUG
	g_global_context.m_editor->GetEditorUI().Draw(); // draw ui
#endif

	g_global_context.m_render_system->Present();
	//g_global_context.m_system->SwapBuffers();

	//auto finish = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	//FOO_LOG("render took %d ms\n", duration.count());
}