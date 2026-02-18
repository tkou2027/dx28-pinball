#include "bumper.h"
#include "component/render/component_light.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "component/physics/component_collider.h"
#include "object/pinball/room.h"
#include "util/tween.h"

namespace
{
	// normalize angle to [-PI, PI]
	static float NormalizeAngle(float a)
	{
		constexpr float TWO_PI = Math::TWO_PI;
		while (a > Math::PI) a -= TWO_PI;
		while (a <= -Math::PI) a += TWO_PI;
		return a;
	}
}

void Bumper::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_render);
	m_components.Add<ComponentCollider>(m_comp_id_collider);
	auto& light = m_components.Add<ComponentLight>(m_comp_id_light);
	light.SetActive(false);
}

void Bumper::Update()
{
	switch (m_state)
	{
	case BumperState::TRANS_IN:
	{
		UpdateTransIn();
		break;
	}
	case BumperState::ACTIVE:
	{
		UpdateActive();
		break;
	}
	case BumperState::TRANS_OUT:
	{
		UpdateTransOut();
		break;
	}
	}
}
void Bumper::InitializeConfig(const BumperConfig& config, std::weak_ptr<Room> room)
{
	m_config = config;
	m_room = room;
	const auto preset = g_bumper_presets[static_cast<size_t>(config.type)];
	m_transform.SetParent(&m_room.lock()->GetTransform());
	m_transform.SetRotationYOnly(config.rotation);
	m_transform.SetPosition(Vector3{ config.position.x, preset.height * 0.5f, config.position.y });
	InitializeVisual();
	InitializeCollision();
	EnterIdle();
}

void Bumper::OnPlayerCollides(bool success)
{
	m_bumped_timer.Initialize(0.3f);
	if (success)
	{
		m_bumped_state = BumpedState::BUMPED_SUCCESS;
	}
	else
	{
		m_bumped_state = BumpedState::BUMPED;
	}
}

void Bumper::ResetSubStates()
{
	m_trans_in_out_factor = 1.0f;
	m_bumped_state = BumpedState::NONE;
}

void Bumper::EnterIdle()
{
	m_state = BumperState::IDLE;
	// disable components
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(false);
	ResetSubStates();
}

void Bumper::EnterActive()
{
	m_state = BumperState::ACTIVE;
	// enable all components
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(true);
	auto& comp_light = m_components.Get<ComponentLight>(m_comp_id_light);
	comp_light.SetActive(true);
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(true);
	// reset states
	ResetSubStates();
	UpdateModelTransInOut();
}

void Bumper::EnterDone()
{
	m_state = BumperState::DONE;
	// disable components
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	auto& comp_light = m_components.Get<ComponentLight>(m_comp_id_light);
	comp_light.SetActive(false);
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(false);
	ResetSubStates();
}

void Bumper::EnterTransIn(float duration)
{
	m_state = BumperState::TRANS_IN;
	// enable visual
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(true);
	auto& comp_light = m_components.Get<ComponentLight>(m_comp_id_light);
	comp_light.SetActive(true);
	ResetSubStates();
	m_state_countdown.Initialize(duration);
}

void Bumper::EnterTransOut(float duration)
{
	m_state = BumperState::TRANS_OUT;
	// disable collider
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(true);
	ResetSubStates();
	m_state_countdown.Initialize(duration);
}

void Bumper::UpdateTransIn()
{
	// update countdown
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterActive();
		return;
	}
	t = 1.0f - t;
	m_trans_in_out_factor = Tween::EaseFunc(Tween::TweenFunction::EASE_OUT_BACK, t);
	UpdateModelTransInOut();
}

void Bumper::UpdateActive()
{
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& pointer_model = comp_render.GetModel(m_pointer_model_index);
	pointer_model.GetTransform().SetRotationYDelta(Math::PI * GetDeltaTime());

	auto& bumper = comp_render.GetModel(m_bumper_model_index);
	m_bumped_timer.Update(GetDeltaTime());
	float t = m_bumped_timer.GetT();
	if (t <= 0.0f)
	{
		m_bumped_state = BumpedState::NONE;
	}
	auto& mat = bumper.GetMaterialDesc().GetTechnique<TechniqueDescDefault>();
	if (m_bumped_state == BumpedState::BUMPED_SUCCESS)
	{
		mat.emission_color = Vector3{ 0.48627f,  0.74902f,  0.74902f } + Vector3{ 1.5f,  0.5f,  0.0f };
	}
	else
	{
		mat.emission_color = Vector3{ 0.48627f,  0.74902f,  0.74902f };
	}
}

void Bumper::UpdateTransOut()
{
	// update countdown
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterDone();
		return;
	}
	// 1 to 0
	m_trans_in_out_factor = Tween::EaseFunc(Tween::TweenFunction::EASE_OUT_QUAD, t);
	UpdateModelTransInOut();
}

void Bumper::UpdateModelTransInOut()
{
	// auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	// auto& pointer_model = comp_render.GetModel(m_pointer_model_index);
	// pointer_model.GetTransform().SetScale(m_trans_in_out_factor);
	// 
	// auto& center_model = comp_render.GetModel(m_bumper_model_index);
	// pointer_model.GetTransform().SetScale(m_trans_in_out_factor);

}

void Bumper::InitializeVisual()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& models = comp_render_mesh.GetModels();
	models.clear(); // reset

	std::string model_key;
	switch (m_config.type)
	{
	case BumperType::TRI_BUMPER:
	{
		model_key = "geo/unit_cylinder_3x1";
		break;
	}
	case BumperType::QUAD_BUMPER:
	{
		model_key = "geo/unit_cylinder_4x1";
		break;
	}
	}

	std::string model_border_key;
	switch (m_config.type)
	{
	case BumperType::TRI_BUMPER:
	{
		model_border_key = "geo/unit_cylinder_side_3x1";
		break;
	}
	case BumperType::QUAD_BUMPER:
	{
		model_border_key = "geo/unit_cylinder_side_4x1";
		break;
	}
	}
	const auto preset = g_bumper_presets[static_cast<size_t>(m_config.type)];
	

	TechniqueDescDefault material_default_common{};
	material_default_common.cull_type = CullType::CULL_BACK;
	material_default_common.shading_model = ShadingModel::UNLIT;
	material_default_common.base_color = Vector3{ 0.48627f,  0.74902f,  0.74902f };
	material_default_common.emission_color = Vector3{ 0.48627f,  0.74902f,  0.74902f };
	material_default_common.emission_intensity = 0.8f;

	// light
	{
		auto& light = m_components.Get<ComponentLight>(m_comp_id_light);
		light.SetActive(true);
		light.SetColor(Vector3{ 0.48627f,  0.74902f,  0.74902f } * 6.0f);	
	}

	// center
	{
		const auto& model_desc = GetPresetManager().GetModelDesc(model_key);
		MaterialDesc material{};
		TechniqueDescDefault material_default{ material_default_common };
		material.SetTechnique(material_default);
		Model model{ model_desc, material, &m_transform };
		float center_padding = Math::Min(0.5f, preset.radius * 0.8f);
		float center_radius = preset.radius - center_padding;
		model.GetTransform().SetScale({ center_radius, preset.height, center_radius });
		m_bumper_model_index = comp_render_mesh.AddModel(model);
	}

	// border
	{
		const int border_num{ 3 };
		const float border_thickness{ 0.5f };
		const float border_step{ border_thickness + 0.2f };
		float border_offset = -(border_num * border_step) * 0.5f; // parent height is center
		const Vector3 border_color[3]{
			{ 0.22745f, 0.61569f, 0.74510f }, // #3a9dbe
			{ 0.35686f, 0.72157f, 0.69804f }, // #5bb8b2
			{ 0.49020f, 0.82745f, 0.76078f }, // #7dd3c2
			// { 0.65882f, 0.94118f, 0.90196f }
		};
		for (int i = 0; i < border_num; i++)
		{
			const auto& model_desc = GetPresetManager().GetModelDesc(model_border_key);
			MaterialDesc material{};
			TechniqueDescDefault material_default{ material_default_common };
			material_default.cull_type = CullType::CULL_NONE;
			material_default.emission_intensity = 0.5f;
			material_default.base_color = border_color[i];
			material_default.emission_color = border_color[i];
			material.SetTechnique(material_default);

			// TODO: color
			Model model{ model_desc, material, &m_transform };
			model.GetTransform().SetScale({ preset.radius, border_thickness, preset.radius });
			model.GetTransform().SetPositionY(border_offset);
			border_offset += border_step;
			comp_render_mesh.AddModel(model);
		}

		// pointer
		{
			const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane_xy");
			MaterialDesc material{};
			TechniqueDescDefault material_default{};
			material_default.no_deferred = true;
			material_default.cull_type = CullType::CULL_NONE;
			material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/bumper_icon.png");
			material_default.pre_pass = false;
			material.SetTechnique(material_default);
			material.SetTechnique(TechniqueDescForwardUnlit{});
			Model model{ model_desc, material, &m_transform };
			model.GetTransform().SetPositionY(preset.height);
			model.GetTransform().SetRotationYOnly(Math::PI * 2.0f);
			model.GetTransform().SetScale({ 2.0f, 2.0f });
			m_pointer_model_index = comp_render_mesh.AddModel(model);
		}
	}
}

void Bumper::InitializeCollision()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	auto& colliders = comp_collider.GetColliders();
	colliders.clear(); // reset

	const auto preset = g_bumper_presets[static_cast<size_t>(m_config.type)];
	// collision
	PolygonCylinder cylinder{};
	cylinder.height = preset.height;
	cylinder.radius = preset.radius;
	cylinder.sides = preset.sides;
	TransformNode3D shape_transform{};
	shape_transform.SetParent(&m_transform);
	Collider collider{ &m_transform, ColliderLayer::Type::BUMPER };
	collider.AddShape(ShapeCollider{ cylinder }, shape_transform);
	collider.bounce = 0.9f;

	float reflection_snap_step{};
	switch (m_config.type)
	{
	case BumperType::TRI_BUMPER:
	{
		reflection_snap_step = Math::PI / 6.0f;
		break;
	}
	case BumperType::QUAD_BUMPER:
	{
		reflection_snap_step = Math::PI / 4.0f;
		break;
	}
	}
	collider.SetSnapReflection(true, reflection_snap_step);

	comp_collider.AddCollider(collider);
}

const Collider& Bumper::GetCollider() const
{
	const auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	return comp_collider.GetCollider(0);
}

Vector3 Bumper::GetSnapPosition(const Vector3& aim_pos)
{
	const auto preset = g_bumper_presets[static_cast<size_t>(m_config.type)];
	const int sides = preset.sides;
	const float radius = preset.radius;


	const Vector3 center_world = m_transform.GetPositionGlobal();

	const Vector3 to_aim_xz = Vector3{ aim_pos.x - center_world.x, 0.0f, aim_pos.z - center_world.z };
	const float rel_len = std::sqrt(to_aim_xz.x * to_aim_xz.x + to_aim_xz.z * to_aim_xz.z);
	if (rel_len < Math::EPSILON)
	{
		return center_world;
	}

	const float aim_angle = std::atan2(to_aim_xz.x, to_aim_xz.z);
	const float rotation_y = m_transform.GetRotationYGlobal();
	const float step = Math::TWO_PI / static_cast<float>(sides);

	// find nearest edge center angle: edge center at rotation_y + (i + 0.5)*step
	int best_index = 0;
	float best_diff = std::numeric_limits<float>::infinity();
	for (int i = 0; i < sides; ++i)
	{
		const float edge_center_angle = rotation_y + (static_cast<float>(i) + 0.5f) * step;
		float diff = std::fabs(NormalizeAngle(aim_angle - edge_center_angle));
		if (diff < best_diff)
		{
			best_diff = diff;
			best_index = i;
		}
	}

	// compute edge center world position
	const float edge_angle = rotation_y + (static_cast<float>(best_index) + 0.5f) * step;
	const float x = std::sinf(edge_angle) * radius * 0.5f;
	const float z = std::cosf(edge_angle) * radius * 0.5f;
	const Vector3 edge_center_world = Vector3{ center_world.x + x, center_world.y, center_world.z + z };
	return edge_center_world;
}
