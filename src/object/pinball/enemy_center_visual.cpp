#include "enemy_center_visual.h"
#include "component/render/component_renderer_mesh.h"
#include "object/game_object_list.h"
#include "object/pinball/screen_square.h"
#include "object/pinball/screen_cylinder.h"
#include "config/camera_names.h"

void EnemyCenterVisual::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
}

void EnemyCenterVisual::InitializeConfig(const EnemyCenterShapeConfig& shape_config)
{
	m_config = shape_config;
	AdjustShapeConfig(m_config);
	m_transform.SetPosition(m_config.position);

	// side screens
	m_screen_squares.clear();
	m_screen_squares.reserve(m_config.sides);
	for (int i = 0; i < m_config.sides; ++i)
	{
		auto screen_square = GetOwner().CreateGameObject<ScreenSquare>();
		const float rotation = static_cast<float>(i) * (Math::TWO_PI / static_cast<float>(m_config.sides));
		const float px = sinf(rotation) * m_config.side_radius * Math::INV_SQRT_2;
		const float pz = cosf(rotation) * m_config.side_radius * Math::INV_SQRT_2;
		screen_square->GetTransform().SetParent(&m_transform);
		screen_square->GetTransform().SetPosition(Vector3{ px, 0.0f, pz });
		screen_square->GetTransform().SetRotationYOnly(rotation);

		float width = m_config.side_radius * Math::SQRT_2; // TODO: adjust width calculation
		screen_square->InitializeConfig(width, m_config.side_height, i, m_config.sides);
		m_screen_squares.push_back(screen_square);
	}

	// status
	auto screen_status = GetOwner().CreateGameObject<ScreenCylinder>();
	screen_status->GetTransform().SetParent(&m_transform);
	screen_status->GetTransform().SetPositionY(m_config.status_position_y);
	screen_status->GetTransform().SetRotationYDelta(Math::PI); // align with side index
	screen_status->InitializeConfig(m_config.status_radius, m_config.status_height);
	m_screen_status = screen_status;

	// borders
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& texture_loader = GetTextureLoader();
	{
		MaterialDesc material_desc{};
		TechniqueDescDefault material_default{};
		material_default.pre_pass = false;

		material_default.metallic = 1.0f;
		material_default.roughness = 1.0f;
		material_default.specular = 1.0f; // ssr

		material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/floor_color_0.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal059A_1K-JPG_NormalDX.jpg");
		material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal059A_1K-JPG_Metalness.jpg");
		material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal059A_1K-JPG_Roughness.jpg");
		material_desc.SetTechnique(material_default);
		{
			// const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_square_ring_bevel");
			MeshGeometry geometry{};
			Geometry::CreateSquareRing(m_config.status_radius, 0.8f, 0.1f, geometry);
			ModelDesc model_desc{};
			auto& model_loader = GetModelLoader();
			model_loader.LoadMeshGeometryUnique(model_desc, geometry);

			const float half_height = m_config.status_height * 0.5f + 0.5f;
			for (int i = 0; i < 2; ++i)
			{
				Model model{ model_desc, material_desc, &m_transform };
				// model.GetTransform().SetScale({ m_config.status_radius, m_config.status_radius, m_config.status_radius });
				model.GetTransform().SetPositionY(m_config.status_position_y + (i == 0 ? half_height : -half_height));
				model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);
				comp_render_mesh.AddModel(model);
			}
		}
		// side borders
		{
			MeshGeometry geometry{};
			Geometry::CreateSquarePillar(m_config.side_height, 1.5f, 0.2f, geometry);
			ModelDesc model_desc{};
			auto& model_loader = GetModelLoader();
			model_loader.LoadMeshGeometryUnique(model_desc, geometry);
			const float half_size = m_config.side_radius * Math::INV_SQRT_2;
			for (int i = 0; i < 4; i++)
			{
				Model model{ model_desc, material_desc, &m_transform };
				// model.GetTransform().SetScale({ m_config.status_radius, m_config.status_radius, m_config.status_radius });
				float x = (i % 2) ? -1.0f : 1.0f;
				float z = (i / 2) ? -1.0f : 1.0f;
				model.GetTransform().SetPosition(Vector3{ x * half_size, 0.0f, z * half_size });
				comp_render_mesh.AddModel(model);
			}
		}
	}
}

void EnemyCenterVisual::AdjustShapeConfig(EnemyCenterShapeConfig& in_out_shape_config)
{
	// adjust height based on texture size
	// cylinder
	const auto& cylinder_camera_preset = g_camera_presets.screen_status;
	float cylinder_aspect_inv = static_cast<float>(cylinder_camera_preset.height) / static_cast<float>(cylinder_camera_preset.width);
	in_out_shape_config.status_height = in_out_shape_config.status_radius * Math::SQRT_2 * 4 * cylinder_aspect_inv;
	// squares
	const auto& square_camera_preset = g_camera_presets.screen_main;
	float square_aspect_inv = static_cast<float>(square_camera_preset.height) / static_cast<float>(square_camera_preset.width);
	in_out_shape_config.side_height = in_out_shape_config.side_radius * Math::SQRT_2 * 4 * square_aspect_inv;
}

void EnemyCenterVisual::SetScreenCrushed(int index)
{
	auto screen_square = m_screen_squares.at(index).lock();
	if (screen_square)
	{
		screen_square->Crush();
	}
}
