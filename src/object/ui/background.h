#pragma once
#include <string>
#include "object/game_object.h"

class Background : public GameObject
{
public:
	void Initialize() override;
	void SetSprite(const std::string& key);
private:
	enum class ButtonEnterState
	{
		IDLE,
		PRESSED
	};
	ComponentId m_comp_id_render{};
};