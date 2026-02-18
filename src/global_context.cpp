#include "global_context.h"
#include "platform/timer.h"
#include "platform/window.h"
#include "platform/keyboard.h"
#include "platform/controller.h"
#include "platform/sound.h"
//#include "platform/graphics.h"
#include "render/render_system.h"
#include "physics/physics_system.h"
#include "config/audio_manager.h"
#include "config/preset_manager.h"
#include "scene/scene_manager.h"
#include "editor/editor.h"

#include "util/debug_ostream.h"

#include <chrono>

GlobalContext g_global_context;

void GlobalContext::StartSystems(const WindowConfig& window_config)
{
	m_timer = std::make_shared<Timer>();
	m_timer->Initialize();

	m_window = std::make_shared<Window>();
	m_window->Initialize(window_config);
	HWND hWnd = m_window->GetHandler();

	// COMèâä˙âª
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	auto start = std::chrono::high_resolution_clock::now();

	m_keyboard = std::make_shared<Keyboard>();
	m_keyboard->Initialize();
	m_controller = std::make_shared<Controller>();
	m_controller->Initialize();

	auto finish = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "init keyboard " << duration.count() << " " << std::endl;

	start = std::chrono::high_resolution_clock::now();

	m_sound = std::make_shared<Sound>();
	m_sound->Initialize(hWnd);

	finish = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "init sound " << duration.count() << " " << std::endl;

	start = std::chrono::high_resolution_clock::now();

	//m_graphics = std::make_shared<Graphics>();
	//m_graphics->Initialize(hWnd);

	finish = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "init graphics " << duration.count() << " " << std::endl;

	start = std::chrono::high_resolution_clock::now();

	m_render_system = std::make_shared<RenderSystem>();
	m_render_system->Initialize(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT);

	m_physics_system = std::make_shared<PhysicsSystem>();
	m_physics_system->Initialize();

	// editor ===
	m_editor = std::make_shared<Editor>();
	m_editor->Initialize();

	// game ====
	m_audio_manager = std::make_shared<AudioManager>();
	m_audio_manager->Initialize();

	m_preset_manager = std::make_shared<PresetManager>();
	m_preset_manager->Initialize();

	m_scene_manager = std::make_shared<SceneManager>();
	m_scene_manager->Initialize();

	finish = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	hal::dout << "init game " << duration.count() << " " << std::endl;
}

void GlobalContext::ShutdownSystems()
{
	m_scene_manager->Finalize();
	m_preset_manager->Finalize();
	m_audio_manager->Finalize();

	m_editor->Finalize();

	m_physics_system->Finalize();
	m_render_system->Finalize();

	//m_graphics->Finalize();
	m_sound->Finalize();
	m_controller->Finalize();
	m_keyboard->Finalize();
	m_window->Finalize();
	m_timer->Finalize();
}
