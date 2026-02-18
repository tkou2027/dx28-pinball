#pragma once
#include <string>
#include <memory>
#include "render/config/sprite_desc.h"
#include "render/config/model_desc.h"

//enum class SpritePresetName
//{
//	BACKGROUND,
//	// block
//	BLOCK_DEFAULT,
//	BLOCK_FALL,
//	BLOCK_ESTIMATION,
//	BLOCK_SOLID,
//	BLOCK_SETTLED_OUT,
//	BLOCK_SETTLED_IN,
//	BLOCK_HEART,
//	// board
//	BOARD_BORDER,
//	// attack
//	ATTACK_LINE,
//	ATTACK,
//	// ui
//	TEXT_PRESS_ENTER,
//	TEXT_PAUSED,
//	TEXT_NUMBER,
//	TEXT_TITLE,
//	// general
//	SOLID,
//	FADE,
//	GRID_BORDER,
//	GRADIENT_HORIZONTAL,
//	// counter
//	MAX
//};
//
//enum class ModelPresetName
//{
//	TEST_MODEL_ANIMATION,
//	TEST_MODEL,
//	TEST_BALL,
//	TEST_AIM,
//	TEST_ENEMY,
//	TEST_BLOCK,
//	TEST_MAP,
//	BLOCK,
//	WALL,
//	// counter
//	MAX
//};

template <typename T>
class ResourcePool;

class PresetManager
{
public:
	PresetManager();
	~PresetManager();
	void Initialize();
	void Finalize() {}; // TODO: release tetures?
	void LoadPresets();
	const ModelDesc& GetModelDesc(const std::string& key) const;
	const SpriteDesc& GetSpriteDesc(const std::string& key) const;
	int GetTextureId(const std::string& key) const;
private:
	void LoadSpritePresets();
	void LoadMeshGeometryPresets();
	void LoadModelPresets();
	void LoadTexturePresets();

	void RegisterModelDesc(const std::string& key, const ModelDesc& desc);
	void RegisterAndLoadModelDesc(const std::string& key, ModelDesc& desc);
	void RegisterAndLoadSpriteDesc(const std::string& key, SpriteDesc& desc);
	void RegisterTexture(const std::string& key, int texture_id);

	std::unique_ptr<ResourcePool<ModelDesc> > m_model_pool; // name -> model desc
	std::unique_ptr<ResourcePool<SpriteDesc> > m_sprite_pool; // name -> sprite desc
	std::unique_ptr<ResourcePool<int> > m_texture_pool; // name -> texture id ?
};