#pragma once
#include "object/game_object.h"

// aiming ui
class PlayerTrail : public GameObject
{
	friend class Player;
public:
	void Initialize() override;
	void SetPlayer(std::weak_ptr<Player> player);
private:
	void UpdateTrail();
	bool UpdateTarget();
	void EnterOrbit();
	void UpdateOrbit();
	void EnterHidden();

	void UpdateSprites(const Vector3& from_pos, const Vector3& to_pos);
	std::weak_ptr<Player> m_player;
	ComponentId m_comp_id_render_trail;
	static constexpr int MAX_TRAIL_SPRITES{ 32 };
};