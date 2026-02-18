#pragma once
//#include "math/vector3.h"
//#include "math/matrix4x4.h"
#include <DirectXMath.h>
#include <windows.h>

static constexpr unsigned int MAX_INSTANCE_PER_DRAW_CALL{ 1024 };

//  constant buffers ====
struct BufferPerProjection
{
	DirectX::XMFLOAT4X4 matrix_proj;
	DirectX::XMFLOAT4X4 matrix_proj_inverse;
};

struct BufferPerProjectionSprite
{
	DirectX::XMFLOAT4X4 matrix_proj;
};

struct BufferPerView
{
	DirectX::XMFLOAT4X4 matrix_view;
	DirectX::XMFLOAT4X4 matrix_view_inverse;
	DirectX::XMFLOAT4X4 matrix_view_proj;
	DirectX::XMFLOAT4X4 matrix_view_proj_inverse;
	DirectX::XMFLOAT3 view_position_w;
	float _padding_view_position_w;
};

struct BufferScreenSize
{
	float screen_width;
	float screen_height;
	float _padding_screen_size_0;
	float _padding_screen_size_1;

};

// per mesh constant buffer
// also used for instancing
struct BufferPerMesh
{
	DirectX::XMFLOAT4X4 matrix_model;
	DirectX::XMFLOAT4X4 matrix_model_inverse_transpose; // normal
	// uv animation
	DirectX::XMFLOAT2 uv_offset;
	DirectX::XMFLOAT2 uv_size;
};

// lights ====
struct LightDirectional
{
	DirectX::XMFLOAT3 direction;
	float _padding_direction;
	DirectX::XMFLOAT3 color;
	float _padding_color;
};

struct LightPoint
{
	DirectX::XMFLOAT3 position_w;
	float _padding_position_w;
	DirectX::XMFLOAT3 color;
	float _padding_color;
};
// TODO othe types of lights

struct BufferLightScene
{
	LightDirectional dir_light;
	LightPoint point_lights[48];
	INT num_point_lights{ 0 };
	float _padding_light_num_1;
	float _padding_light_num_2;
	float _padding_light_num_3;
};

// shadow mapping ====
struct LightDirectionalShadow
{
	DirectX::XMFLOAT4X4 matrix_light_view_proj;
};

struct BufferLightShadowScene
{
	LightDirectionalShadow dir_light_shadow;
};

// materials ====
struct MaterialDefault
{
	DirectX::XMFLOAT4 base_color;

	float metallic;
	float specular;
	float roughness;
	UINT shading_model;

	DirectX::XMFLOAT3 emission_color;
	float emission_intensity;
};

// constant buffers ====
struct DirectionalLight
{
	DirectX::XMFLOAT3 direction;
	float _padding_direction;
};

struct BufferPerFrame
{
	// view
	DirectX::XMFLOAT4X4 matrix_view_proj;
	DirectX::XMFLOAT4X4 matrix_view_proj_no_transform;
	DirectX::XMFLOAT4X4 matrix_view;
	// camera
	DirectX::XMFLOAT3 camera_position;
	float _camera_position_padding;
	// light
	DirectionalLight directional_light;
	DirectX::XMFLOAT4X4 matrix_directional_light_view_proj;
};

// materials ====
// shadow

// cel
struct MaterialCel
{
	float shader_mode; // TODO: this should be removed
	float ramp_offset; // TODO: this should be sampled from texture
	float rim_radius;
	float rim_intensity;

	float receive_shadow;
	float use_alter_texture; // TODO: this should be in a different pass?
	float _padding_1;
	float _padding_2;
};

struct BufferPerMaterialCel
{
	MaterialCel material_cel;
};

// outline
struct MaterialOutline
{
	DirectX::XMFLOAT4 color;
	float width;
	float _width_padding_1;
	float _width_padding_2;
	float _width_padding_3;
};

struct BufferPerMaterialOutline
{
	MaterialOutline material_outline;
};

// mask
struct MaterialMask
{
	float mask;
	float _padding_1;
	float _padding_2;
	float _padding_3;
};

struct BufferPerMaterialMask
{
	MaterialMask material_mask;
};

// vertex and index buffers ====
