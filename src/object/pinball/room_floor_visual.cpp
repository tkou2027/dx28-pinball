#include "room_floor_visual.h"

#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "render/resource/model_loader.h"
#include "config/preset_manager.h"
#include "math/hex.h"

void RoomFloorVisual::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
}

void RoomFloorVisual::InitializeConfig(const FloorConfig& config)
{
	m_config = config;
	InitializeFloorFace();
	InitializeFloorBorder();
	InitializeFloorSide();
}

void RoomFloorVisual::SetVisible(bool visible)
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	comp_render_mesh.SetActive(visible);
}

void RoomFloorVisual::InitializeFloorFace()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	// floor face
	{
		HexTileMap hex_map{};
		hex_map.Initialize(m_config.hex_size, HexCoord::GenerateSpiral(20));
		MeshGeometry geometry{};
		Geometry::CreateHexTileMap(hex_map, geometry);

		ModelDesc model_desc{};
		auto& model_loader = GetModelLoader();
		model_loader.LoadMeshGeometryUnique(model_desc, geometry);

		auto& texture_loader = GetTextureLoader();

		MaterialDesc material_desc{};
		TechniqueDescDefault material_default{};
		material_default.pre_pass = false;

		material_default.metallic = 1.0f;
		material_default.roughness = 1.0f;
		material_default.specular = 1.0f; // ssr

		material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/floor_color_0.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_256-JPG_NormalDX.jpg");
		material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_256-JPG_Metalness.jpg");
		material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_256-JPG_Roughness.jpg");

		//material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Color.jpg");
		//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_NormalDX.jpg");
		//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Metalness.jpg");
		//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Roughness.jpg");

		//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_NormalDX.jpg");
		//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_Metalness.jpg");
		//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_Roughness.jpg");

		//material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_1K-JPG_Color.jpg");


		//material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Color.jpg");
		// material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_NormalDX.jpg");
		////material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Roughness.jpg", true);
		//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Roughness.jpg", true);


		// material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Poliigon_ClayCeramicGlossy_5212_Normal.png");
		//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Poliigon_ClayCeramicGlossy_5212_Metallic.jpg", true);
		//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Poliigon_ClayCeramicGlossy_5212_Roughness.jpg", true);

		material_desc.SetTechnique(material_default);

		TechniqueDescDeferredFloor deferred_floor{};
		deferred_floor.radius_inner = m_config.radius_inner * Math::INV_SQRT_2;
		deferred_floor.radius_outer = m_config.radius_outer * Math::INV_SQRT_2;
		material_desc.SetTechnique(deferred_floor);

		Model model{ model_desc, material_desc, &m_transform };
		model.GetUVAnimationState().uv_scroll_size = Vector2{ 1.0f, 1.0f } * 0.2f;
		comp_render_mesh.AddModel(model);
	}

}

void RoomFloorVisual::InitializeFloorBorder()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	// floor border
	// outer ring
	{
		const auto& model_desc{ GetPresetManager().GetModelDesc("geo/unit_cylinder_side_4x1") };

		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		//material_default.emission_color = Vector3{ 1.00000f,  0.75294f, 0.79608f };
		material_default.base_color = Vector3{ 1.0f, 1.0f, 1.0f };
		material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
		material_default.emission_intensity = 0.0f;
		material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/goal.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		material_default.emission_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/goal.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		material_default.cull_type = CullType::CULL_NONE;
		material_default.shading_model = ShadingModel::UNLIT;

		material.SetTechnique(material_default);

		Model model{ model_desc, material, &m_transform };

		model.GetTransform().SetScale({ m_config.radius_outer, 0.5f,  m_config.radius_outer });
		model.GetTransform().SetPosition({ 0.0f, 0.25f, 0.0f });
		model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);

		comp_render_mesh.AddModel(model);
	}
}

void RoomFloorVisual::InitializeFloorSide()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& texture_loader = GetTextureLoader();
	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.metallic = 0.1f;
	material_default.roughness = 1.0f;
	//material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Color.jpg", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_NormalDX.jpg");
	//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Metalness.jpg");
	//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Roughness.jpg");
	// material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Tiles132A_1K-JPG_Color.jpg", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/floor_color_2.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Tiles132A_1K-JPG_NormalDX.jpg");
	//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Tiles132A_1K-JPG_Roughness.jpg");
	// material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_NormalDX.jpg");
	//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Roughness.jpg");
	//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Roughness.jpg");
	// material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal059A_1K-JPG_NormalDX.jpg");
	// material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal059A_1K-JPG_Metalness.jpg");
	// material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal059A_1K-JPG_Roughness.jpg");
	material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_NormalDX.jpg");
	material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Metalness.jpg");
	material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Roughness.jpg");
	material_desc.SetTechnique(material_default);

	{
		// outer side
		ModelDesc model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_4x1");;

		Model model{ model_desc, material_desc, &m_transform };
		model.GetTransform().SetScale(Vector3{ m_config.radius_outer, m_config.side_height, m_config.radius_outer });
		model.GetTransform().SetPositionY(m_config.side_height * -0.5f);
		model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);
		model.GetUVAnimationState().uv_scroll_size = Vector2{ (4.0f * Math::SQRT_2 * m_config.radius_outer) / m_config.side_height, 1.0f } *3.0f;

		comp_render_mesh.AddModel(model);
	}

	if (m_config.radius_inner > 0.0f)
	{
		// inner side
		ModelDesc model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_reversed_4x1");

		Model model{ model_desc, material_desc, &m_transform };
		model.GetTransform().SetScale(Vector3{ m_config.radius_inner, m_config.side_height, m_config.radius_inner });
		model.GetTransform().SetPositionY(m_config.side_height * -0.5f);
		model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);
		// model.GetUVAnimationState().uv_scroll_size = Vector2{ 130.0f, 160.0f };
		model.GetUVAnimationState().uv_scroll_size = Vector2{ (4.0f * Math::SQRT_2 * m_config.radius_inner) / m_config.side_height, 1.0f } *3.0f;

		comp_render_mesh.AddModel(model);
	}
}


