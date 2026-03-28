#include "player_title.h"
#include "object/pinball/player.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"

void PlayerTitle::Initialize()
{
	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
	const auto& model_desc = GetPresetManager().GetModelDesc("model/character_blink");
	MaterialDesc material{};
	Player::CreateMaterialDesc(material);
	Model model{ model_desc, material, &m_transform };
	comp_render_mesh.AddModel(model);
}
