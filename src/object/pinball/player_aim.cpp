#include "player_aim.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "object/pinball/player.h"
#include "object/pinball/bumper.h"
// physics
#include "physics/physics_scene.h"


void PlayerAim::Initialize()
{

	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane");
	MaterialDesc material_common{};
	TechniqueDescDefault material_common_default{};
	material_common_default.no_deferred = true;
	material_common.SetTechnique(material_common_default);
	material_common.SetTechnique(TechniqueDescForwardUnlit{});
	// circle
	{
		MaterialDesc material{ material_common };
		auto& material_default = material.GetTechnique<TechniqueDescDefault>();
		material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/shadow.png");
		material_default.pre_pass = false;
		Model model{ model_desc, material, nullptr };
		model.GetTransform().SetPositionY(0.01f);
		model.GetTransform().SetScale({ 3.0f, 1.0f, 3.0f });
		auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render_circle);
		comp_render_mesh.AddModel(model);
	}
	// dots
	{
		MaterialDesc material{ material_common };
		auto& material_default = material.GetTechnique<TechniqueDescDefault>();
		material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/aim.png");
		material_default.pre_pass = false;
		auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render_aim);
		for (int i = 0; i < m_config.max_dot_sprites; i++)
		{
			Model model{ model_desc, material, &m_transform };
			model.GetTransform().SetPositionY(0.01f);
			model.GetTransform().SetScale({ m_config.dot_size, 1.0f,  m_config.dot_size });
			model.SetActive(false);
			comp_render_mesh.AddModel(model);
		}
	}

}

void PlayerAim::SetPlayer(std::weak_ptr<Player> player)
{
	m_player = player;
	auto& comp_render_circle = m_components.Get<ComponentRendererMesh>(m_comp_id_render_circle);
	auto& circle_model = comp_render_circle.GetModel(0);
	circle_model.GetTransform().SetParent(&m_player.lock()->GetTransform());
}

bool PlayerAim::UpdateTarget()
{
	auto player = m_player.lock();
	const Vector3& from_pos = player->GetTransform().GetPosition();
	//const Vector3& target_dir = player->GetTransform().GetForwardGlobal();
	//Ray ray{ from_pos, target_dir };
	auto& physics_scene = GetPhysicsScene();
	CollisionInfo collision_info{};
	//bool hit = physics_scene.CastRayXZ(
	//	ray, m_config.max_aim_distance, ColliderLayer::MASK_BUMPER, collision_info);

	bool hit = physics_scene.ClosestHit(
		from_pos, m_config.max_aim_distance, ColliderLayer::MASK_BUMPER, collision_info);

	// update target
	if (!hit)
	{
		m_target_bumper.reset();
		// update visual
		auto& comp_render_aiming = m_components.Get<ComponentRendererMesh>(m_comp_id_render_aim);
		comp_render_aiming.SetActive(false);
		return false;
	}
	// update target
	assert(collision_info.other); // make visual studio happy
	m_target_bumper = std::dynamic_pointer_cast<Bumper>(collision_info.other->GetOwner().lock());

	// update visual
	auto& comp_render_aiming = m_components.Get<ComponentRendererMesh>(m_comp_id_render_aim);
	const Vector3& target_pos = collision_info.hit_info.hit_position;
	UpdateSprites(from_pos, target_pos);
	return true;

	//float to_target_length = (target_pos - from_pos).Length();
	//int num_dots = static_cast<int>(floorf(to_target_length / m_config.dot_distance));
	//const Vector3 to_target_dir = (target_pos - from_pos).GetNormalized();
	//const Vector3 dot_offset = to_target_dir * m_config.dot_distance;
	//const Vector3 dot_start_pos = from_pos + to_target_dir * m_config.dot_distance * 0.5f;

	//for (int i = 0; i < m_config.max_dot_sprites; i++)
	//{
	//	auto& dot = comp_render_aiming.GetModel(i);
	//	if (i < num_dots)
	//	{
	//		dot.SetActive(true);
	//		Vector3 dot_pos = dot_start_pos + dot_offset * (i + 1);
	//		dot.GetTransform().SetPosition(dot_pos);
	//	}
	//	else
	//	{
	//		dot.SetActive(false);
	//	}
	//}
	
}

bool PlayerAim::UpdateOrbit()
{
	auto target = m_target_bumper.lock();
	if (!target || !target->IfActive())
	{
		m_target_bumper.reset();
		return false;
	}

	const auto& bumper_collider = target->GetCollider();

	// get target border
	const Vector3& from_pos = m_player.lock()->GetTransform().GetPosition();
	Vector3 target_pos = target->GetSnapPosition(from_pos); // target->GetTransform().GetPositionGlobal();
	Vector3 to_target = target_pos - from_pos;
	to_target.y = 0.0f;

	auto& physics_scene = GetPhysicsScene();
	Ray to_target_ray{ from_pos, to_target.GetNormalized() };
	CollisionInfo collision_info{};
	bool hit = physics_scene.CastRayXZ(
		to_target_ray, to_target.Length() + 2.0f, bumper_collider, collision_info);

	auto& comp_render_aiming = m_components.Get<ComponentRendererMesh>(m_comp_id_render_aim);
	if (!hit)
	{
		comp_render_aiming.SetActive(false);
		return false;
	}
	UpdateSprites(from_pos, collision_info.hit_info.hit_position);

	return true;
}



void PlayerAim::EnterHidden()
{
	auto& comp_render_aiming = m_components.Get<ComponentRendererMesh>(m_comp_id_render_aim);
	comp_render_aiming.SetActive(false);
}

void PlayerAim::UpdateSprites(const Vector3& from_pos, const Vector3& target_pos)
{
	// update visual
	auto& comp_render_aim = m_components.Get<ComponentRendererMesh>(m_comp_id_render_aim);
	comp_render_aim.SetActive(true);
	float to_target_length = (target_pos - from_pos).Length();
	int num_dots = static_cast<int>(floorf(to_target_length / m_config.dot_distance));
	const Vector3 to_target_dir = (target_pos - from_pos).GetNormalized();
	const Vector3 dot_offset = to_target_dir * m_config.dot_distance;
	const Vector3 dot_start_pos = from_pos + to_target_dir * m_config.dot_distance * 0.5f + Vector3{ 0.0f, 0.01f, 0.0f };

	for (int i = 0; i < m_config.max_dot_sprites; i++)
	{
		auto& dot = comp_render_aim.GetModel(i);
		if (i < num_dots)
		{
			dot.SetActive(true);
			Vector3 dot_pos = dot_start_pos + dot_offset * (i + 1);
			dot.GetTransform().SetPosition(dot_pos);
		}
		else
		{
			dot.SetActive(false);
		}
	}
}
