#pragma once
#include "object/game_object.h"

class PlayerUI : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void SetPlayer(std::weak_ptr<class Player> player) { m_player = player; }
private:
	std::weak_ptr<class Player> m_player;
};