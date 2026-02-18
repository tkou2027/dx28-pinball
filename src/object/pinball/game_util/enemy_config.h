#pragma once
#include <vector>
#include "bumper_config.h"

enum class EnemyAttackType
{
	BUMPER,
};

struct EnemyAttackConfig
{
	// action
	EnemyAttackType type{ EnemyAttackType::BUMPER };
	std::vector<BumperConfig> bumpers{};
	
	float prepare_duration{ 0.1f };
	float follow_duration{ 0.1f };
	float execute_duration{ 0.1f };
	float cooldown_duration{ 15.0f };
};

struct EnemyAttackPhaseConfig
{
	std::vector<EnemyAttackConfig> attacks{};
};

struct EnemyCenterShapeConfig
{
	Vector3 position{ 0.0f, 0.0f, 0.0f };
	float side_radius{ 30.0f };
	float side_height{ 120.0f };
	float status_radius{ 40.0f };
	float status_height{ 5.0f };
	float status_position_y{ 30.0f };
	int sides{ 4 };
};

struct EnemyCenterAimUIConfig
{
	// aim ui
	float center_height{ 0.5f };
	float radius_min{ 0.0f };
	float radius_max{ 0.9f };
	float thickness_min{ 0.0f };
	float thickness_max{ 0.4f };
};

struct EnemyCenterConfig
{
	// shape
	EnemyCenterShapeConfig shape{};
	EnemyCenterAimUIConfig aim_ui{};
	// status
	float health{ 100.0f };
	float damage_delay{ 1.0f };
	float exit_duration{ 1.0f };
	Vector3 aim_offset{ 0.0f, 1.5f, 0.0f }; // player height offset
	// attacks
	std::vector<EnemyAttackPhaseConfig> attack_phases{};
};
