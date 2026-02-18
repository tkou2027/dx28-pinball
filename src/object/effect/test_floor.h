#pragma once
#include "object/game_object.h"
#include "component/render/component_renderer_mesh.h"
#include "render/resource/texture_loader.h"
#include "render/render_resource.h"
#include "config/preset_manager.h"

#include "math/hex.h"
#include "render/resource/model_loader.h"

class TestFloor : public GameObject
{
public:
	void Initialize() override
	{
		auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);

		float inner_radius = 40.0f;
		float outer_radius = 75.0f;

		{
			HexTileMap hex_map{};
			hex_map.Initialize(4.0f, HexCoord::GenerateSpiral(16, 4));
			MeshGeometry geometry{};
			Geometry::CreateHexTileMap(hex_map, geometry);

			ModelDesc model_desc{};
			auto& model_loader = GetModelLoader();
			model_loader.LoadMeshGeometryUnique(model_desc, geometry);

			auto& texture_loader = GetTextureLoader();

			MaterialDesc material_desc{};
			TechniqueDescDefault material_default{};
			material_default.metallic = 0.7f;
			material_default.roughness = 1.0f;
			//material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Color.jpg");
			//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_NormalDX.jpg", true);
			//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Metalness.jpg", true);
			//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament018_1K-JPG_Roughness.jpg", true);

			//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_NormalDX.jpg", true);
			//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_Metalness.jpg", true);
			//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament021_1K-JPG_Roughness.jpg", true);

			//material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_1K-JPG_Color.jpg");
			material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_256-JPG_NormalDX.jpg");
			material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_256-JPG_Metalness.jpg");
			material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Metal029_256-JPG_Roughness.jpg");

			//material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Color.jpg");
			//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_NormalDX.jpg", true);
			////material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Roughness.jpg", true);
			//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Plastic017A_1K-JPG_Roughness.jpg", true);


			material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/floor_color_0.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			//material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Poliigon_ClayCeramicGlossy_5212_Normal.png", true);
			//material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Poliigon_ClayCeramicGlossy_5212_Metallic.jpg", true);
			//material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/Poliigon_ClayCeramicGlossy_5212_Roughness.jpg", true);

			material_default.specular = 1.0f; // ssr

			material_default.cull_type = CullType::CULL_BACK;
			material_desc.SetTechnique(material_default);

			TechniqueDescDeferredFloor deferred_floor{};
			deferred_floor.radius_inner = inner_radius;
			deferred_floor.radius_outer = outer_radius;
			material_desc.SetTechnique(deferred_floor);

			Model model{ model_desc, material_desc, &m_transform };

			// m_transform.SetPositionY(0.5f);

			comp_render_mesh.AddModel(model);
		}

		{
			// inner center
			ModelDesc model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_reversed_4x1");

			auto& texture_loader = GetTextureLoader();
			MaterialDesc material_desc{};
			TechniqueDescDefault material_default{};
			material_default.metallic = 1.0f;
			material_default.roughness = 1.0f;
			material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Color.jpg", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_NormalDX.jpg");
			material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Metalness.jpg");
			material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Roughness.jpg");
			// material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/verticle.png");
			//material_default.emission_color = Vector3{ 1.0f, 0.0f, 0.0f };
			//material_default.emission_intensity = 1.0f;
			//material_default.no_deferred = true;

			material_default.cull_type = CullType::CULL_BACK;
			material_desc.SetTechnique(material_default);
			//material_desc.SetTechnique(TechniqueDescForwardUnlit{});


			Model model{ model_desc, material_desc, &m_transform };
			model.GetTransform().SetScale(Vector3{ inner_radius * sqrtf(2), 80.0f, inner_radius * sqrtf(2) });
			model.GetTransform().SetPositionY(-40.0f);
			model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);

			//model.GetUVAnimationState().uv_scroll_size = Vector2{ 1.0f, 1.0f } * inner_radius * 30.0f;
			model.GetUVAnimationState().uv_scroll_size = Vector2{ 30.0f, 60.0f };

			comp_render_mesh.AddModel(model);
		}

		{

			ModelDesc model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_4x1");;

			auto& texture_loader = GetTextureLoader();
			MaterialDesc material_desc{};
			TechniqueDescDefault material_default{};
			material_default.metallic = 1.0f;
			material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Color.jpg", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_NormalDX.jpg");
			material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Metalness.jpg");
			material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Roughness.jpg");

			material_default.cull_type = CullType::CULL_BACK;
			material_desc.SetTechnique(material_default);


			Model model{ model_desc, material_desc, &m_transform };
			model.GetTransform().SetScale(Vector3{ outer_radius, 80.0f, outer_radius });
			model.GetTransform().SetPositionY(-40.0f);
			model.GetUVAnimationState().uv_scroll_size = Vector2{ 45.0f, 60.0f };
			model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);

			comp_render_mesh.AddModel(model);
		}

		//{

		//	ModelDesc model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_128x2");;

		//	auto& texture_loader = GetTextureLoader();
		//	MaterialDesc material_desc{};
		//	TechniqueDescDefault material_default{};
		//	material_default.metallic = 1.0f;
		//	material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Color.jpg");
		//	material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_NormalDX.jpg");
		//	material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Metalness.jpg");
		//	material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Roughness.jpg");

		//	material_default.cull_type = CullType::CULL_BACK;
		//	material_desc.SetTechnique(material_default);


		//	Model model{ model_desc, material_desc, &m_transform };
		//	model.GetTransform().SetScale(Vector3{ outer_radius, 80.0f, outer_radius });
		//	model.GetTransform().SetPositionY(-40.0f);
		//	model.GetUVAnimationState().uv_scroll_size = Vector2{ 45.0f, 60.0f };

		//	comp_render_mesh.AddModel(model);
		//}

		//{
		//	ModelDesc model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_6x2");;

		//	auto& texture_loader = GetTextureLoader();
		//	MaterialDesc material_desc{};
		//	TechniqueDescDefault material_default{};
		//	material_default.metallic = 1.0f;
		//	material_default.roughness = 1.0f;
		//	material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Color.jpg");
		//	material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_NormalDX.jpg");
		//	material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Metalness.jpg");
		//	material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Roughness.jpg");

		//	material_default.cull_type = CullType::CULL_FRONT;
		//	material_desc.SetTechnique(material_default);


		//	Model model{ model_desc, material_desc, &m_transform };
		//	model.GetTransform().SetScale(Vector3{ 150.0f, 80.0f, 150.0f });
		//	model.GetTransform().SetPositionY(-30.0f);
		//	model.GetTransform().SetRotationYOnly(Math::PI / 6.0f);
		//	model.GetUVAnimationState().uv_scroll_size = Vector2{ 60.0f, 60.0f };

		//	comp_render_mesh.AddModel(model);
		//}

		//{
		//	HexTileMap hex_map{};
		//	hex_map.Initialize(8.0f, HexCoord::GenerateSpiral(20));
		//	MeshGeometry geometry{};
		//	Geometry::CreateHexTileMap(hex_map, geometry);

		//	ModelDesc model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_side_6x2");;

		//	auto& texture_loader = GetTextureLoader();
		//	MaterialDesc material_desc{};
		//	TechniqueDescDefault material_default{};
		//	material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Color.jpg");
		//	material_default.normal_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_NormalDX.jpg");
		//	material_default.metallic_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Metalness.jpg");
		//	material_default.roughness_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/pbr/ChristmasTreeOrnament020_1K-JPG_Roughness.jpg");

		//	material_default.cull_type = CullType::CULL_FRONT;
		//	material_desc.SetTechnique(material_default);


		//	Model model{ model_desc, material_desc, &m_transform };
		//	model.GetTransform().SetScale(Vector3{ 150.0f, 50.0f, 150.0f });
		//	model.GetTransform().SetRotationYOnly(Math::PI / 6.0f);

		//	comp_render_mesh.AddModel(model);
		//}
	}
private:
	ComponentId m_comp_id_mesh{};
};