#include "window_controller.h"
#include "global_context.h"
#include "platform/controller.h"
#include "platform/keyboard.h"
#include "platform/window.h"

void WindowController::Initialize()
{
	m_full_screen = false;
}

void WindowController::Update()
{
	if (g_global_context.m_keyboard->IsKeyTrigger(KK_F11))
	{
		m_full_screen = !m_full_screen;
		g_global_context.m_window->SetFullScreen(m_full_screen);
	}
}