#include "screen_main_ui.h"
#include "component/render/component_renderer_sprite.h"
#include "component/render/component_renderer_mesh.h"
#include "config/camera_names.h"
#include "config/preset_manager.h"
#include "config/palette.h"
#include "render/resource/texture_loader.h"

#include "object/game_object_list.h"
#include "object/pinball/camera_follow.h"
#include "component/render/component_camera.h"

using namespace DirectX;

void ScreenMainUI::Initialize()
{
	m_components.Add<ComponentRendererSprite>(m_comp_id_sprite);
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);

	{
		auto& ui_config = m_config.animation_configs[static_cast<size_t>(ActiveState::COOLDOWN)];
		ui_config.max_radius = 0.2f;
		ui_config.min_radius = 0.02f;
		ui_config.max_thickness = 0.08f;
		ui_config.min_thickness = 0.04f;
		ui_config.animation_interval = 4.0f;
		ui_config.color_inside = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
		ui_config.color_stripe_positive = g_palette.color_theme;
		ui_config.color_stripe_negative = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
	}
	{
		auto& ui_config = m_config.animation_configs[static_cast<size_t>(ActiveState::HURT)];
		ui_config.max_radius = 1.0f;
		ui_config.min_radius = 0.0f;
		ui_config.max_thickness = 0.0f;
		ui_config.min_thickness = 1.0f;
		ui_config.animation_interval = 1.0f;
		ui_config.color_inside = Vector4{ 0.0f, 0.0f, 0.0f, 0.0f }; // mask
		ui_config.color_stripe_positive = g_palette.color_theme;
		ui_config.color_stripe_negative = g_palette.color_theme;
	}
}

void ScreenMainUI::InitializeConfig(const EnemyCenterShapeConfig& shape_config)
{
	m_config.shape_height = shape_config.side_height;
	m_config.shape_radius = shape_config.side_radius;
	m_config.shape_position = shape_config.position;
	InitializeSprites();
	SetSpritesSize();
}

void ScreenMainUI::Update()
{
	switch (m_state)
	{
	case State::IDLE:
	{
		break;
	}
	case State::ACTIVE:
	{
		UpdateAnimation();
		UpdateProjection();
		break;
	}
	}
}

void ScreenMainUI::EnterActiveState(ActiveState state)
{
	m_state = State::ACTIVE;
	m_active_state = state;
	const auto& animation_config = m_config.animation_configs[static_cast<size_t>(m_active_state)];
	m_animation_timer.Initialize(animation_config.animation_interval);
	SetBackgroundPalette();
}

void ScreenMainUI::UpdateAnimation()
{
	float t = m_animation_timer.Update(GetDeltaTime());
	// update animation
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(m_id_mesh_background);
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);

	auto& animation_config = m_config.animation_configs[static_cast<size_t>(m_active_state)];
	mat.radius = Math::Lerp(animation_config.max_radius, animation_config.min_radius, t);
	mat.thickness = Math::Lerp(animation_config.min_thickness, animation_config.max_thickness, t);
	if (m_active_state == ActiveState::COOLDOWN)
	{
		mat.rotation = t * Math::TWO_PI;
	}
}

void ScreenMainUI::UpdateProjection()
{
	auto camera_main = GetOwner().FindGameObject<CameraFollow>();
	const auto& camera_comp_id = camera_main->GetComponents().TryGetComponent<ComponentCamera>();
	const auto& comp_camera_main = GetOwner().GetComponentManager().GetComponent<ComponentCamera>(camera_comp_id);
	const Vector3 main_position = camera_main->GetTransform().GetPositionGlobal();
	const Vector3 main_target = comp_camera_main.GetTarget();
	const Vector3 main_up = comp_camera_main.GetUp();
	const auto& main_shape_config = comp_camera_main.GetShapeConfig();
	XMMATRIX mat_view = CameraMath::CalculateViewMatrix(main_position, main_target, main_up);
	XMMATRIX mat_proj = CameraMath::CalculateProjectionMatrix(main_shape_config);
	XMMATRIX mat_view_proj = mat_view * mat_proj;

	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	for (int i = 0; i < NUM_SIDES; i++)
	{
		auto& material_desc = comp_mesh.GetModel(m_id_mesh_projections[i]).GetMaterialDesc();
		auto& material_desc_projection = material_desc.GetTechnique<TechniqueDescForwardScreenProjection>(CameraRenderLayer::SCREEN_MAIN);
		XMStoreFloat4x4(&material_desc_projection.projector_mat_view_proj, XMMatrixTranspose(mat_view_proj));
	}
}

void ScreenMainUI::SetAimInfo(const AimInfo& aim_info)
{
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(m_id_mesh_background);
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.center_u = aim_info.rotation_ratio;
	mat.center_v = aim_info.height_ratio;
}

void ScreenMainUI::InitializeSprites()
{
	auto& preset_manager = GetPresetManager();
	auto& texture_loader = GetTextureLoader();
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane_xy");
		MaterialDesc material_desc{};
		TechniqueDescForwardScreenBackground material_screen_bg{};
		material_screen_bg.radius = 0.0f;
		material_screen_bg.thickness = 0.0f;
		material_screen_bg.color_inside = g_palette.color_theme;
		material_screen_bg.color_stripe_positive = g_palette.color_theme;
		material_screen_bg.color_stripe_negative = g_palette.color_theme;
		material_desc.SetTechnique(material_screen_bg, CameraRenderLayerMask::SCREEN_MAIN);
		Model model{ model_desc, material_desc, &m_transform };
		m_id_mesh_background = mesh_comp.AddModel(model);
	}
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane_xy");
		const float half_height = m_config.shape_height * 0.5f;
		//  2
		// 1 3
		//  0
		const float rotation_offset = Math::PI * 0.75f; // start screen 3 top left
		const float rotation_step = -Math::TWO_PI / static_cast<float>(NUM_SIDES);
		// fill screen with 4 planes
		auto& camera_preset = g_camera_presets.screen_main;
		const float screen_width = static_cast<float>(camera_preset.width);
		const float screen_height = static_cast<float>(camera_preset.height);
		const float screen_width_step = screen_width / static_cast<float>(NUM_SIDES);
		const float screen_width_offset = screen_width_step * (-static_cast<float>(NUM_SIDES) * 0.5f + 0.5f);
		for (int i = 0; i < NUM_SIDES; i++)
		{
			MaterialDesc material_desc{};
			TechniqueDescForwardScreenProjection material_screen_projection{};
			material_screen_projection.position_w_top_left = Vector3{
				m_config.shape_radius * sinf(rotation_offset + rotation_step * i),
				half_height,
				m_config.shape_radius* cosf(rotation_offset + rotation_step * i)
			} + m_config.shape_position;
			material_screen_projection.position_w_bottom_right = Vector3{
				m_config.shape_radius * sinf(rotation_offset + rotation_step * (i + 1)),
				-half_height,
				m_config.shape_radius * cosf(rotation_offset + rotation_step * (i + 1))
			} + m_config.shape_position;
			material_screen_projection.world_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.monitor.name);

			material_desc.SetTechnique(material_screen_projection, CameraRenderLayerMask::SCREEN_MAIN);

			Model model{ model_desc, material_desc, &m_transform };
			model.GetTransform().SetScale(Vector3{ screen_width_step, screen_height, 1.0f});
			model.GetTransform().SetPosition(Vector3{ screen_width_step * i + screen_width_offset, 0.5f });
			m_id_mesh_projections[i] = mesh_comp.AddModel(model);
		}
	}
}

void ScreenMainUI::SetBackgroundPalette()
{
	auto& ui_config = m_config.animation_configs[static_cast<size_t>(m_active_state)];
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(m_id_mesh_background);
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.color_inside = ui_config.color_inside;
	mat.color_stripe_positive = ui_config.color_stripe_positive;
	mat.color_stripe_negative = ui_config.color_stripe_negative;
}

void ScreenMainUI::SetSpritesSize()
{
	auto& camera_preset = g_camera_presets.screen_main;
	const float screen_width = static_cast<float>(camera_preset.width);
	const float screen_height = static_cast<float>(camera_preset.height);
	const float center_x = screen_width * 0.5f;
	// mesh
	const float center_y = screen_height * 0.5f;
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(m_id_mesh_background);
	model.GetTransform().SetScale({ screen_width, screen_height, 1.0f });
	model.GetTransform().SetPosition({ center_x, center_y, 0.5f }); // 0.5f to avoid clipping
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
}
