#pragma once
#include <Windows.h>
#include <string>

struct WindowConfig
{
	HINSTANCE hInstance;
	int nCmdShow;
	std::string title{ "DirectX" };
	std::string window_class{ "GameWindow" };
};