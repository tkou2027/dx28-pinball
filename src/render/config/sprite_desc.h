#pragma once
#include <string>
#include <vector>
#include "math/vector2.h"
#include "math/vector4.h"
#include "math/transform2d.h"
#include "render/config/uv_animation_state.h"
#include "render/config/texture_resource_id.h"
#include "render/config/camera_render_layer.h"

enum class SpriteLayer
{
	DEFAULT,
	EFFECT,
	UI,
	MAX
};

struct SpriteDesc
{
	// textures
	std::string texture_path{};
	TextureResourceId texture_id{}; // to be computed
	// color
	Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	bool blend_add{ false };
	// transform
	Vector2 size{ 1.0f, 1.0f };
	Vector2 offset{};
	// uv transform
	std::vector<UVFrameAnimationDesc> uv_animation_desc{};
	// uv scroll
	Vector2 initial_uv_scroll_size{ 1.0f, 1.0f };
	Vector2 initial_uv_scroll_offset{ 0.0f, 0.0f };

	uint32_t render_layer_mask{ CameraRenderLayerMask::DEFAULT };

	//// uv transform
	//Vector2 uv_size{ 1.0f, 1.0f };
	//Vector2 uv_offset{ 0.0f, 0.0f };
	//Vector2 uv_size_per_frame{ 1.0f, 1.0f }; // to be computed
	//// uv animation
	//int frame_cols{ 1 };
	//int frame_rows{ 1 };
	//int frame_total{ 1 };
	//int frame_offset{ 0 };
	//int frame_default{ 0 };
	//int play_speed_scale{ 1 };
	//bool play_loop{ false };
};

struct SpriteOutput
{
	// textures
	TextureResourceId texture_id{};
	Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	// ordering
	SpriteLayer layer{ SpriteLayer::DEFAULT };
	// transform
	Transform2D transform;
	// local size
	Vector2 size{ 1.0f, 1.0f };
	Vector2 offset{};
	// uv transform
	Vector2 uv_size{ 1.0f, 1.0f };
	Vector2 uv_offset{ 0.0f, 0.0f };
	// blend
	bool blend_add{ false };
	// camera render layer mask
	uint32_t render_layer_mask{ CameraRenderLayerMask::DEFAULT };
};

class Sprite
{
public:
	Sprite() = default;
	Sprite(const SpriteDesc& desc);
	void Initialize(const SpriteDesc& desc);
	void SetDesc(const SpriteDesc& desc);
	void Update();
	bool GetOutput(SpriteOutput& sprite_output);

	bool m_active{ false };
	// referencing 
	SpriteDesc m_desc{};
	TransformNode2D m_transform{};
	UVAnimationState m_uv_animation_state{};

	//int m_frame{ 0 };
	//int m_play_count{ 0 };
	//bool m_playing{ true };
	//// overwrite input
	//Vector2 m_size{ 1.0f, 1.0f };
	//Vector2 m_offset{};
	//Vector2 m_uv_size{ 1.0f, 1.0f };
	//Vector2 m_uv_offset{}; // uv scrolling, not compatible with griding and fliping
	//bool m_blend_add{ false };

};