#pragma once
#pragma once
#include "scene.h"

class SceneTest : public Scene
{
public:
	void Initialize() override;
private:
	void InitializeTestScreen();
	void InitializeTestTexture();
};