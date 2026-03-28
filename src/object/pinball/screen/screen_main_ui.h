#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"
#include "object/pinball/game_util/enemy_config.h"
#include "util/yoyo_timer.h"

class ScreenMainUI : public GameObject
{
public:
	struct AimInfo
	{
		float rotation_ratio{ 0.0f }; // u
		float height_ratio{}; // v
	};
	enum class ActiveState
	{
		COOLDOWN,
		HURT,
		MAX
	};
	void Initialize() override;
	void InitializeConfig(const EnemyCenterShapeConfig& shape_config);
	void Update() override;
	void EnterActiveState(ActiveState state);
	void SetAimInfo(const AimInfo& aim_info);
private:
	void UpdateAnimation();
	void UpdateProjection();
	void InitializeSprites();
	void SetBackgroundPalette();
	void SetSpritesSize();
	ComponentId m_comp_id_sprite{};
	ComponentId m_comp_id_mesh{};
	int m_id_mesh_background{};
	static const int NUM_SIDES{ 4 };
	int m_id_mesh_projections[NUM_SIDES]{};

	// status
	enum class State
	{
		IDLE,
		ACTIVE
	};
	State m_state{ State::ACTIVE };
	ActiveState m_active_state{ ActiveState::COOLDOWN };
	// status
	YoyoTimer m_animation_timer{};
	CountdownTimer m_delay_timer{};

	struct UIAnimationConfig
	{
		float max_radius{ 1.0f };
		float min_radius{ 0.0f };
		float max_thickness{ 0.5f };
		float min_thickness{ 0.04f };
		float animation_interval{ 2.0f };
		Vector4 color_inside{};
		Vector4 color_stripe_positive{};
		Vector4 color_stripe_negative{};
	};
	struct UIConfig
	{
		float shape_radius;
		float shape_height;
		Vector3 shape_position;
		UIAnimationConfig animation_configs[static_cast<size_t>(ActiveState::MAX)];
	};
	UIConfig m_config{};
};