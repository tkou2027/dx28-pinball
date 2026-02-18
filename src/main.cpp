#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "application.h"
#include "global_context.h"
#include "config/window_config.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	WindowConfig window_config{
		hInstance,
		nCmdShow
	};
	Application app{};
	app.Initialize(window_config);
	app.Run();
	app.Finalize();
}
