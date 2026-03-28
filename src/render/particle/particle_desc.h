#pragma once
#include "math/vector2.h"
#include "math/vector3.h"
#include "render/config/texture_resource_id.h"
#include "render/config/uv_animation_desc.h"
#include "particle_emitter_id.h"

enum class EmitterType
{
    TEXTURE_MESH
};

enum class TextureParticleShape
{
    PLANE = 0,
    CYLINDER = 1,
    // other shapes...
};

struct EmitterDesc
{
    ParticleEmitterId id{ ParticleEmitterIdAllocator::ID_INVALID };
    // TODO: shape type
    TextureParticleShape shape;
    unsigned int num_width;
    unsigned int num_height;
    Vector2 local_mesh_size{};

    TextureResourceId texture_id{};
    TextureResourceId local_texture_id{};

	UVRect texture_uv{};

    // update
	float lifetime{ 1.0f };
	Vector3 gravity{ 0.0f, -9.8f, 0.0f };
};

enum class EmitterState
{
    IDLE,
    INITIALIZED,
    BURST_INIT,
    BURST_UPDATE,
};

struct EmitterInitData
{
    EmitterDesc desc{};
    DirectX::XMFLOAT4X4 transform{};
};

struct EmitterUpdateData
{
    ParticleEmitterId id{};
    EmitterState state{};
	float delta_time{};
    UVRect texture_uv{};
};
