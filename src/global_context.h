#pragma once
#include <Windows.h>
#include <memory>
#include "config/window_config.h"
// platform
class Timer;
class Window;
class Keyboard;
class Controller;
class Sound;
// render
class RenderSystem;
// physics
class PhysicsSystem;

// editor
class Editor;
// game
class AudioManager;
class PresetManager;
class SceneManager;

class GlobalContext
{
public:
	void StartSystems(const WindowConfig& window_config);
	void ShutdownSystems();

	// systems
	std::shared_ptr<Timer> m_timer{ nullptr };
	std::shared_ptr<Window> m_window{ nullptr };
	std::shared_ptr<Keyboard> m_keyboard{ nullptr };
	std::shared_ptr<Controller> m_controller{ nullptr };
	std::shared_ptr<Sound> m_sound{ nullptr };
	// std::shared_ptr<Graphics> m_graphics{ nullptr };
	std::shared_ptr<RenderSystem> m_render_system{ nullptr };
	std::shared_ptr<PhysicsSystem> m_physics_system{ nullptr };
	// editor
	std::shared_ptr<Editor> m_editor{ nullptr };
	// game
	std::shared_ptr<AudioManager> m_audio_manager{ nullptr };
	std::shared_ptr<PresetManager> m_preset_manager{ nullptr };
	std::shared_ptr<SceneManager> m_scene_manager{ nullptr };
};

extern GlobalContext g_global_context;