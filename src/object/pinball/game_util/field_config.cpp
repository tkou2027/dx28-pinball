#include "field_config.h"

namespace
{
	//static float RadiusToHalfSize(float radius)
	//{
	//	return radius * Math::INV_SQRT_2;
	//}

	// floor setting
	static RoomConfig CreateFloor0()
	{
		RoomConfig room{};
		room.floor.radius_outer = 48.0f;
		room.floor.radius_inner = 0.0f;

		const float half_size = room.floor.radius_outer * Math::INV_SQRT_2;
		room.camera_center = Vector3{ 0.0f, room.floor.height, 200.0f };
		room.player_trans_in_position = Vector3{ 0.0f, room.floor.height, -half_size + 5.0f };
		room.player_active_position = Vector3{ 0.0f, room.floor.height, -half_size + 15.0f };

		// bumpers
		{
			auto bumper_preset_tri = g_bumper_presets[static_cast<size_t>(BumperType::TRI_BUMPER)];
			const float bumper_distance = half_size * Math::INV_SQRT_3;// +bumper_preset_tri.radius;
			room.bumpers.push_back(
				BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f, Vector2{ bumper_distance, bumper_preset_tri.radius * 0.5f } }
			);
			room.bumpers.push_back(
				BumperConfig{ BumperType::TRI_BUMPER, 0.0f, Vector2{ -bumper_distance, 0.0f } }
			);
		}
		// goals
		//{
		//	room.goals.push_back(
		//		GoalConfig{
		//			GoalType::DEFAULT,
		//			3,  // speed level
		//			6.0f,  // radius
		//			15.0f,  // height
		//			{ 0.0f, half_size } // position
		//		}
		//	);
		//}
		{
			EnemyCenterConfig enemy{};
			room.enemies_center.push_back(enemy);
		}
		return room;
	}

	static RoomConfig CreateFloor1()
	{
		RoomConfig room{};
		room.floor.radius_outer = 100.0f;
		room.floor.radius_inner = 48.0f;
		float camera_dist = -160.0f;
		room.camera_center = Vector3{ 0.0f, room.floor.height, -160.0f };
		room.player_active_position = Vector3{ 0.0f, room.floor.height, room.floor.radius_inner + 10.0f };
		const float to_center = room.floor.radius_outer * 1.5f;

		const float center_step = to_center / 6.0f;


		// bumpers
		{
			auto bumper_preset_tri = g_bumper_presets[static_cast<size_t>(BumperType::TRI_BUMPER)];
			auto bumper_preset_qua = g_bumper_presets[static_cast<size_t>(BumperType::QUAD_BUMPER)];
			const float qua_size_half = bumper_preset_qua.radius * Math::INV_SQRT_2;
			room.bumpers.push_back(
				BumperConfig{ BumperType::QUAD_BUMPER, Math::PI / 4.0f, Vector2{ center_step + qua_size_half, 0.0f } }
			);
			room.bumpers.push_back(
				BumperConfig{ BumperType::QUAD_BUMPER, Math::PI / 4.0f, Vector2{ -center_step - qua_size_half , center_step * 2.0f } }
			);
			room.bumpers.push_back(
				BumperConfig{ BumperType::QUAD_BUMPER, Math::PI / 4.0f, Vector2{ -center_step - qua_size_half, -center_step * 2.0f } }
			);

			//floor.bumpers.push_back(
			//	BumperConfig{ BumperType::TRI_BUMPER, 0.0f, Vector2{ center_step * 4, -center_step * 3.0f } }
			//);
			//floor.bumpers.push_back(
			//	BumperConfig{ BumperType::TRI_BUMPER, -Math::PI / 6.0f, Vector2{ 12.0f, 0.0f } }
			//);
			//floor.bumpers.push_back(
			//	BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 6.0f, Vector2{ -12.0f, 0.0f } }
			//);
			//floor.bumpers.push_back(
			//	BumperConfig{ BumperType::TRI_BUMPER, -Math::PI / 6.0f, Vector2{ 20.0f, -16.0f } }
			//);
			//floor.bumpers.push_back(
			//	BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 6.0f, Vector2{ -20.0f, -16.0f } }
			//);
		}
		// goals
		{
			const float goal_radius = 18.0f;
			const float goal_height = 16.0f;
			room.goals.push_back(
				GoalConfig{
					GoalType::DEFAULT,
					4,  // speed level
					goal_radius,  // radius
					goal_height,  // height
					{ 0.0f, -to_center } // position
				}
			);
			//floor.goals.push_back(
			//	GoalConfig{
			//		GoalType::DEFAULT,
			//		3,  // speed level
			//		goal_radius,  // radius
			//		goal_height,  // height
			//		{ to_center * 0.5f, -to_center * SQRT_3 * 0.5f } // position
			//	}
			//);
			//floor.goals.push_back(
			//	GoalConfig{
			//		GoalType::DEFAULT,
			//		3,  // speed level
			//		goal_radius,  // radius
			//		goal_height,  // height
			//		{ -to_center * 0.5f, -to_center * SQRT_3 * 0.5f } // position
			//	}
			//);
		}
		return room;
	}

	static RoomConfig CreateFloor2()
	{
		RoomConfig room{};
		room.floor.radius_outer = 120.0f;
		room.floor.radius_inner = 50.0f;

		float camera_dist = -160.0f;
		room.camera_center = Vector3{ 0.0f, 0.0f, 0.0f };//160.0f };
		room.player_active_position = Vector3{ 0.0f, room.floor.height, -(room.floor.radius_inner + 10.0f) };
		const float to_center = room.floor.radius_outer * 1.5f;

		const float center_step = to_center / 6.0f;


		// bumpers
		//{
		//	auto bumper_preset_tri = g_bumper_presets[static_cast<size_t>(BumperType::TRI_BUMPER)];
		//	auto bumper_preset_qua = g_bumper_presets[static_cast<size_t>(BumperType::QUAD_BUMPER)];
		//	const float qua_size_half = bumper_preset_qua.radius * Math::INV_SQRT_2;

		//	room.bumpers.push_back(
		//		BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 0.0f, Vector2{ 0.0f, -(room.floor.radius_inner * 1.5f + 15.0f) } }
		//	);
		//	room.bumpers.push_back(
		//		BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 1.0f, Vector2{ -(room.floor.radius_inner * 1.5f + 15.0f), 0.0f } }
		//	);
		//	room.bumpers.push_back(
		//		BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 0.0f, Vector2{ (room.floor.radius_inner * 1.5f + 15.0f), 0.0f } }
		//	);
		//	room.bumpers.push_back(
		//		BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 1.0f, Vector2{ 0.0f, (room.floor.radius_inner * 1.5f + 15.0f) } }
		//	);

		//}
		// goals
		//{
		//	const float goal_radius = 18.0f;
		//	const float goal_height = 16.0f;

		//	room.goals.push_back(
		//		GoalConfig{
		//			GoalType::DEFAULT,
		//			1,  // speed level
		//			goal_radius * 1,  // radius
		//			goal_height,  // height
		//			{ 0.0f, 0.0f } // position
		//		}
		//	);
		//}
		// center enemies
		{

			float status_aspect_inv = 24.0f / 1024.0f;
			EnemyCenterConfig enemy{};
			//enemy.shape.position = Vector3{ 0.0f, 0.0f, 0.0f };
			//enemy.shape.side_radius = 30.0f;
			//enemy.shape.side_height = 80.0f;
			//enemy.shape.status_radius = 40.0f;
			enemy.shape.status_height = enemy.shape.status_radius * Math::SQRT_2 * 4 * status_aspect_inv;
			//enemy.shape.status_position_y = 30.0f;
			//enemy.shape.sides = 4;
			//enemy.health = 100.0f;

			EnemyAttackPhaseConfig phase_0{};
			EnemyAttackPhaseConfig phase_1{};
			// attacks
			{
				EnemyAttackConfig attack{};
				attack.type = EnemyAttackType::BUMPER;
				//attack.prepare_duration = 0.5f;
				//attack.execute_duration = 1.0f;
				//attack.cooldown_duration = 10.0f;
				attack.bumpers.push_back(
					BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 0.0f, Vector2{ 0.0f, -(room.floor.radius_inner + 15.0f) } }
				);
				attack.bumpers.push_back(
					BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 2.0f * 1.0f, Vector2{ -(room.floor.radius_inner + 15.0f), 0.0f } }
				);
				attack.bumpers.push_back(
					BumperConfig{ BumperType::TRI_BUMPER, -Math::PI / 2.0f * 1.0f, Vector2{ (room.floor.radius_inner + 15.0f), 0.0f } }
				);
				attack.bumpers.push_back(
					BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 1.0f, Vector2{ 0.0f, (room.floor.radius_inner + 15.0f) } }
				);

				phase_0.attacks.push_back(attack);
			}
			//{
			//	EnemyAttackConfig attack{};
			//	attack.type = EnemyAttackType::BUMPER;
			//	//attack.prepare_duration = 0.5f;
			//	//attack.execute_duration = 1.0f;
			//	//attack.cooldown_duration = 10.0f;
			//	attack.bumpers.push_back(
			//		BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 0.0f, Vector2{ 0.0f, -(room.floor.radius_inner + 15.0f) } }
			//	);
			//	attack.bumpers.push_back(
			//		BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 2.0f * 1.0f, Vector2{ -(room.floor.radius_inner + 15.0f), 0.0f } }
			//	);
			//	attack.bumpers.push_back(
			//		BumperConfig{ BumperType::TRI_BUMPER, -Math::PI / 2.0f * 1.0f, Vector2{ (room.floor.radius_inner + 15.0f), 0.0f } }
			//	);
			//	attack.bumpers.push_back(
			//		BumperConfig{ BumperType::TRI_BUMPER, Math::PI / 3.0f * 1.0f, Vector2{ 0.0f, (room.floor.radius_inner + 15.0f) } }
			//	);

			//	phase_0.attacks.push_back(attack);
			//}
			enemy.attack_phases.push_back(phase_0);
			// enemy.attack_phases.push_back(phase_1);
			room.enemies_center.push_back(enemy);
		}


		return room;
	}
} // namespace

FieldConfig g_field_config
{
	{
		// CreateFloor0(),
		// CreateFloor1(),
		CreateFloor2(),
		// CreateFloor0(),
		// CreateFloor1(),

	}
};