#pragma once
#include <DirectXMath.h>
#include "render/particle/particle_desc.h"
#include "render/resource/model_data.h"

struct EmitterData
{
	EmitterDesc desc{};
	DirectX::XMFLOAT4X4 transform;
    BufferView buffer_particles{};
	int model_id{ -1 };
};

struct TextureParticleData
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 velocity;
    //DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 uv;
    // DirectX::XMFLOAT4 color;

	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;

    float life;
    float delay;
};
