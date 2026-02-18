#include "player_trail.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "object/pinball/player.h"

void PlayerTrail::Initialize()
{
	// trail model
	{
		const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane");
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/air.png");
		material_default.no_deferred = true;
		material.SetTechnique(material_default);
		material.SetTechnique(TechniqueDescForwardUnlit{});
		auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render_trail);
		for (int i = 0; i < MAX_TRAIL_SPRITES; i++)
		{
			MaterialDesc material{ material };
			auto& material_default = material.GetTechnique<TechniqueDescDefault>();
			Model model{ model_desc, material, &m_transform };
			model.GetTransform().SetPositionY(0.01f);
			model.GetTransform().SetScale({ 1.0f, 1.0f, 1.0f });
			model.SetActive(false);
			comp_render_mesh.AddModel(model);
		}
	}
}

void PlayerTrail::SetPlayer(std::weak_ptr<Player> player)
{
	m_player = player;
}

void PlayerTrail::UpdateTrail()
{
	// update trail position
	auto player = m_player.lock();
	assert(player);
	const Vector3& player_pos = player->GetTransform().GetPositionGlobal();
	m_transform.SetPosition(player_pos);
	// update sprites
}