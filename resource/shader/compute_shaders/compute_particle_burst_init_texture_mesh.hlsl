#include "texture_particle.hlsli"

RWStructuredBuffer<TextureParticle> g_particles : register(u0);

cbuffer UpdateParams : register(b0)
{
    float g_dt;
    float3 g_gravity;
    uint g_total_particles;
};

Texture2D g_texture_noise : register(t0);
SamplerState g_sampler : register(s0);

[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint index = id.x;
    if (index >= g_total_particles)
    {
        return;
    }

    TextureParticle p = g_particles[index];

    // initialize

    g_particles[index] = p;
}