#pragma once
#include <Windows.h>
#include "config/window_config.h"
class Application
{
public:
	void Initialize(const WindowConfig& window_config);
	int Run();
	void Finalize();
private:
	void Update();
	void Draw();
};