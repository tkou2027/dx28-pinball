#include "preset_manager.h"
#include "global_context.h"
#include "render/render_system.h"
#include "config/constant.h"
#include "config/palette.h"
#include "render/render_resource.h"
#include "util/resource_pool.h"

PresetManager::PresetManager() = default;
PresetManager::~PresetManager() = default;

void PresetManager::Initialize()
{
	m_model_pool = std::make_unique<ResourcePool<ModelDesc> >();
	m_sprite_pool = std::make_unique<ResourcePool<SpriteDesc> >();
	m_texture_pool = std::make_unique<ResourcePool<int> >();
}

void PresetManager::LoadPresets()
{
	LoadSpritePresets();
	LoadMeshGeometryPresets();
	LoadModelPresets();
	LoadTexturePresets();
}

//const SpriteDesc& PresetManager::LoadSpritesPreset(SpritePresetName name)
//{
//	SpriteDesc& preset = m_sprite_presets[static_cast<size_t>(name)];
//	// load texture
//	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
//	preset.texture_id = texture_loader.GetOrLoadTextureFromFile(preset.texture_path);
//
//	// split tile sheet
//	preset.uv_size_per_frame.x = preset.uv_size.x / preset.frame_cols;
//	preset.uv_size_per_frame.y = preset.uv_size.y / preset.frame_rows;
//
//	return preset;
//}

//void PresetManager::InitializeSpritePresets()
//{
//	SpriteDesc* preset{ nullptr };
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BACKGROUND);
//	preset->texture_path = "asset/texture/game_bg.png";
//	preset->size = { SCREEN_WIDTH, SCREEN_HEIGHT };
//	preset->offset = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
//
//	// block ====
//	const std::string block_texture_path{ "asset/texture/blocks.png" };
//	int block_cols{ 6 };
//	Vector2 block_size{ 30, 30 };
//	Vector4 block_color{ g_palette.color_theme };
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BLOCK_DEFAULT);
//	preset->texture_path = block_texture_path;
//	preset->size = block_size;
//	preset->color = block_color;
//	preset->frame_cols = block_cols;
//	preset->frame_total = 1;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BLOCK_FALL);
//	preset->texture_path = block_texture_path;
//	preset->size = block_size;
//	preset->color = block_color;
//	preset->frame_cols = block_cols;
//	preset->frame_offset = 1;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BLOCK_ESTIMATION);
//	preset->texture_path = block_texture_path;
//	preset->size = block_size;
//	preset->color = block_color;
//	preset->frame_cols = block_cols;
//	preset->frame_offset = 2;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BLOCK_SOLID);
//	preset->texture_path = block_texture_path;
//	preset->size = block_size;
//	preset->color = block_color;
//	preset->frame_cols = block_cols;
//	preset->frame_offset = 3;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BLOCK_SETTLED_OUT);
//	preset->texture_path = block_texture_path;
//	preset->size = block_size;
//	preset->color = block_color;
//	preset->frame_cols = block_cols;
//	preset->frame_offset = 4;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BLOCK_SETTLED_IN);
//	preset->texture_path = block_texture_path;
//	preset->size = block_size;
//	preset->color = block_color;
//	preset->frame_cols = block_cols;
//	preset->frame_offset = 5;
//
//	// health
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BLOCK_HEART);
//	preset->texture_path = "asset/texture/hearts.png";
//	preset->size = { 30 - 4, 30 - 4 };
//	preset->color = g_palette.color_danger;
//	preset->frame_cols = 4;
//	preset->frame_total = 4;
//
//	// board ====
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::BOARD_BORDER);
//	preset->texture_path = "asset/texture/block_border.png";
//	preset->size = { 30, 30 };
//	preset->color = g_palette.color_theme;
//
//	// attack ===
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::ATTACK_LINE);
//	preset->texture_path = "asset/texture/attack_line.png";
//	preset->size = { 30, 10.0f };
//	preset->color = g_palette.color_danger;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::ATTACK);
//	preset->texture_path = "asset/texture/attack.png";
//	preset->size = { 30, 30 };
//	preset->color = g_palette.color_danger;
//
//	// UI
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::TEXT_PRESS_ENTER);
//	preset->texture_path = "asset/texture/text_press_enter.png";
//	preset->size = Vector2{ 150.0f, 5.0f } * 4.0f;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::TEXT_PAUSED);
//	preset->texture_path = "asset/texture/text_paused.png";
//	preset->size = Vector2{ 42.0f, 5.0f } * 10.0f;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::TEXT_NUMBER);
//	preset->texture_path = "asset/texture/block_numbers_gb_09.png";
//	preset->size = Vector2{ 8.0f, 16.0f } * 3.0f;
//	preset->frame_cols = 10;
//	preset->frame_total = 10;
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::TEXT_TITLE);
//	preset->texture_path = "asset/texture/text_title.png";
//	preset->size = Vector2{ 128.0f, 16.0f } * 4.0f;
//
//	// general
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::SOLID);
//	preset->texture_path = "asset/texture/solid.png";
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::GRADIENT_HORIZONTAL);
//	preset->texture_path = "asset/texture/gradient_horizontal.png";
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::GRID_BORDER);
//	preset->texture_path = "asset/texture/tiles.png";
//	preset->size = { 30, 30 };
//	preset->color = Vector4{ 0.0f, 0.0f, 0.0f, 0.2f };
//
//	preset = m_sprite_presets + static_cast<size_t>(SpritePresetName::FADE);
//	preset->texture_path = "asset/texture/fade.png";
//	preset->color = preset->color = g_palette.color_theme;
//	preset->size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
//	preset->uv_size = { 128.0f, 1.0f };
//}

void PresetManager::LoadSpritePresets()
{
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/text_press_enter.png";
		preset.size = Vector2{ 150.0f, 5.0f } *4.0f;
		RegisterAndLoadSpriteDesc("sprite/ui/button_enter", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/fade.png";
		preset.color = g_palette.color_theme;
		preset.size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		preset.initial_uv_scroll_offset = { 128.0f, 1.0f };
		RegisterAndLoadSpriteDesc("sprite/ui/fade", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/title_bg.png";
		preset.size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		RegisterAndLoadSpriteDesc("sprite/ui/bg_title", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ending_bg.png";
		preset.size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		RegisterAndLoadSpriteDesc("sprite/ui/bg_ending", preset);
	}
	// status sprites ====
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_border.png";
		preset.color = g_palette.color_theme;
		preset.render_layer_mask = CameraRenderLayerMask::SCREEN_STATUS;
		RegisterAndLoadSpriteDesc("sprite/status/border", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_bar_back.png";
		preset.color = g_palette.color_theme;
		preset.render_layer_mask = CameraRenderLayerMask::SCREEN_STATUS;
		RegisterAndLoadSpriteDesc("sprite/status/bar_back", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_bar_value.png";
		preset.color = g_palette.color_theme;
		preset.render_layer_mask = CameraRenderLayerMask::SCREEN_STATUS;
		RegisterAndLoadSpriteDesc("sprite/status/bar_value", preset);
	}
	// status sprites end ====
	// screen main sprites ====
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_border.png";
		preset.color = g_palette.color_theme;
		preset.render_layer_mask = CameraRenderLayerMask::SCREEN_MAIN;
		RegisterAndLoadSpriteDesc("sprite/main/border", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/testmask.png";
		// preset.color = g_palette.color_theme;
		preset.render_layer_mask = CameraRenderLayerMask::SCREEN_MAIN;
		RegisterAndLoadSpriteDesc("sprite/main/testmask", preset);
	}
	// main sprites end ====
}

void PresetManager::LoadMeshGeometryPresets()
{
	auto& model_loader = g_global_context.m_render_system->GetRenderResource().GetModelLoader();
	{
		ModelDesc preset{};
		preset.model_file = "unit_cube";
		MeshGeometry geometry{};
		Geometry::CreateBox(1.0f, 1.0f, 1.0f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_plane";
		MeshGeometry geometry{};
		Geometry::CreatePlane(1.0f, 1.0f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_plane_xy";
		MeshGeometry geometry{};
		Geometry::CreatePlaneXY(1.0f, 1.0f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_3x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinder(1.0f, 1.0f, 3, 1, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_3x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 3, 1, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_4x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinder(1.0f, 1.0f, 4, 1, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_4x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 4, 1, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_reversed_4x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 4, 1, true, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_6x2";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 6, 2, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_32x2";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 32, 2, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_128x2";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 128, 2, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_square_ring_bevel";
		MeshGeometry geometry{};
		Geometry::CreateSquareRing(1.0f, 0.2f, 0.05f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
}

void PresetManager::LoadModelPresets()
{

	{
		ModelDesc preset{};
		//preset.model_file = "asset/models/character.fbx";
		preset.model_file = "asset/models/primitive/iso_sphere.fbx";
		RegisterAndLoadModelDesc("model/primitive/iso_sphere", preset);
	}
	{
		ModelDesc preset{};
		//preset.model_file = "asset/models/character.fbx";
		preset.model_file = "asset/models/primitive/tv.fbx";
		RegisterAndLoadModelDesc("model/tv_face", preset);
	}
	{
		ModelDesc preset{};
		//preset.model_file = "asset/models/character.fbx";
		preset.model_file = "asset/models/primitive/tv_box.fbx";
		RegisterAndLoadModelDesc("model/tv_box", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/vampire/dancing_vampire.dae";
		// preset.model_file = "asset/models/character/BaseCharacter.fbx";
		preset.has_animation = true;
		AnimationDesc animation_desc{};
		animation_desc.animation_file = "asset/models/vampire/dancing_vampire.dae";
		// animation_desc.animation_file = "asset/models/character/BaseCharacter.fbx";
		animation_desc.play_loop = true;
		preset.animations_desc.push_back(animation_desc);
		RegisterAndLoadModelDesc("model/test_animation", preset);
		// RegisterAndLoadModelDesc("model/character", preset);
	}
	{
		ModelDesc preset{};
		//preset.model_file = "asset/models/character.fbx";
		preset.model_file = "asset/models/character/chara_anim2.fbx";
		//RegisterAndLoadModelDesc("model/character", preset);

		preset.has_animation = true;
		AnimationDesc animation_desc{};
		animation_desc.animation_file = "asset/models/character/chara_anim2.fbx";
		// animation_desc.animation_file = "asset/models/character/BaseCharacter.fbx";
		animation_desc.play_loop = true;
		preset.animations_desc.push_back(animation_desc);
		RegisterAndLoadModelDesc("model/character", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hex/test.fbx";
		RegisterAndLoadModelDesc("model/block", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hex/hexwall.fbx";
		RegisterAndLoadModelDesc("model/wall", preset);
	}
}

void PresetManager::LoadTexturePresets()
{

}

const ModelDesc& PresetManager::GetModelDesc(const std::string& key) const
{
	int id = m_model_pool->GetId(key);
	assert(id >= 0);
	return m_model_pool->Get(id);
}

const SpriteDesc& PresetManager::GetSpriteDesc(const std::string& key) const
{
	int id = m_sprite_pool->GetId(key);
	assert(id >= 0);
	return m_sprite_pool->Get(id);
}

int PresetManager::GetTextureId(const std::string& key) const
{
	int id = m_texture_pool->GetId(key);
	assert(id >= 0);
	return m_texture_pool->Get(id);
}

void PresetManager::RegisterModelDesc(const std::string& key, const ModelDesc& desc)
{
	m_model_pool->AddIfNotExists(key, desc);
}

void PresetManager::RegisterAndLoadModelDesc(const std::string& key, ModelDesc& desc)
{
	auto& model_loader = g_global_context.m_render_system->GetRenderResource().GetModelLoader();
	// load models
	model_loader.GetOrLoadModel(desc);
	// register
	m_model_pool->AddIfNotExists(key, desc);
}

void PresetManager::RegisterAndLoadSpriteDesc(const std::string& key, SpriteDesc& preset)
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	preset.texture_id = texture_loader.GetOrLoadTextureFromFile(preset.texture_path, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	// split tile sheet
	for (auto& uv_desc : preset.uv_animation_desc)
	{
		uv_desc.uv_size_per_frame.x = uv_desc.uv_size.x / uv_desc.frame_cols;
		uv_desc.uv_size_per_frame.y = uv_desc.uv_size.y / uv_desc.frame_rows;
	}
	// register
	m_sprite_pool->AddIfNotExists(key, preset);
}

void PresetManager::RegisterTexture(const std::string& key, int texture_id)
{
	m_texture_pool->AddIfNotExists(key, texture_id);
}
