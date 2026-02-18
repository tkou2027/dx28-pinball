#pragma once
#include "object/game_object.h"

// aiming ui
class Bumper;
class PlayerAim : public GameObject
{
	friend class Player;
public:
	void Initialize() override;
	void SetPlayer(std::weak_ptr<Player> player);
	Bumper& GetTargetBumper() const { return *m_target_bumper.lock(); }
private:
	bool UpdateTarget();
	bool UpdateOrbit();
	void EnterHidden();

	void UpdateSprites(const Vector3& from_pos, const Vector3& to_pos);
	struct AimConfig
	{
		float max_aim_distance{ 25.0f };
		float dot_distance{ 2.0f };
		float dot_size{ 1.0f };
		int max_dot_sprites{ 80 };// static_cast<int>(ceilf(max_aim_distance / dot_distance)
	};
	AimConfig m_config{};
	std::weak_ptr<Bumper> m_target_bumper;
	std::weak_ptr<Player> m_player;
	ComponentId m_comp_id_render_aim;
	ComponentId m_comp_id_render_circle;
	static constexpr int MAX_AIM_SPRITES{ 32 };
};