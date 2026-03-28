#include "texture_particle.hlsli"

RWStructuredBuffer<TextureParticle> g_particles : register(u0);

cbuffer UpdateParams : register(b0)
{
    float g_dt;
    float3 g_gravity;
    float g_life;
    float3 g_world_center;
    float2 g_world_scale;
    uint g_total_particles;
    float _update_params_padding_1;
};

Texture2D g_texture_noise : register(t0);
SamplerState g_sampler : register(s0);

// TODO
float Hash(uint n)
{
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return float(n & 0x7fffffffU) / float(0x7fffffff);
}

float SampleNoise(float2 p)
{
    float color = g_texture_noise.SampleLevel(
        g_sampler,
        p,
        0
    );
    color = color * 2.0 - 1.0;
    return color;
}

float2 Curl2D(float2 p)
{
    float delta = 1.0 / 256.0;

    float n = SampleNoise(p + float2(0, delta));
    float s = SampleNoise(p - float2(0, delta));
    float w = SampleNoise(p - float2(delta, 0));
    float e = SampleNoise(p + float2(delta, 0));

    float dx = (e - w);
    float dy = (n - s);
    return float2(dy, -dx) / (2.0 * delta);
}

float2 Gradient2D(float2 p)
{
    float delta = 1.0 / 256.0;

    float n = SampleNoise(p + float2(0, delta));
    float s = SampleNoise(p - float2(0, delta));
    float w = SampleNoise(p - float2(delta, 0));
    float e = SampleNoise(p + float2(delta, 0));

    float dx = (e - w);
    float dy = (n - s);
    return normalize(float2(dx, dy) / (2.0 * delta));
}

float2 CurlAndGradient2D(float2 p)
{
    float delta = 1.0 / 256.0;
    float n = SampleNoise(p + float2(0, delta));
    float s = SampleNoise(p - float2(0, delta));
    float w = SampleNoise(p - float2(delta, 0));
    float e = SampleNoise(p + float2(delta, 0));

    float dx = (e - w);
    float dy = (n - s);
    float2 curl = float2(dy, -dx) / (2.0 * delta);
    float2 gradient = float2(dx, dy) / (2.0 * delta);
    return curl + gradient;
}

float2 GetWorldSpaceUV(float3 position_w, float3 right, float3 up)
{
    float2 uv;
    uv.x = dot(position_w, right);
    uv.y = dot(position_w, up);
    uv = uv * g_world_scale; // scale to dynamic uv
    
    uv = uv + 0.5f; // map to [0,1]
    return uv;
}

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
    if (p.life > 1000.0f)
    {
        if (p.delay > 0.0f)
        {
            p.delay -= g_dt * 2.0f;
        }
        else
        {
            float3 g_hit_pos = float3(0.0, 0.0, 0.0);
            float g_hit_force = 10.0f;
            float rnd = Hash(index);
            float3 dir = -normalize(p.position - g_hit_pos);
            // p.velocity = dir * (g_hit_force + rnd * 10.0f);
            p.velocity.y += -30.0f + rnd * 30.0f;
            p.life = g_life;
        }

    }
    else if (p.life > 0.0f)
    {
        p.velocity += g_gravity * p.up * g_dt;

        float2 world_uv = GetWorldSpaceUV(p.position - g_world_center, p.right, p.up);
        float2 curl = CurlAndGradient2D(world_uv);
        float3 flow = curl.x * p.right+ (curl.y * 0.5 - 0.5) * p.up;
        p.velocity += flow * g_dt;

        p.velocity.x *= 0.98;
        p.velocity.z *= 0.98;
        p.position += p.velocity * g_dt;
        p.life -= g_dt;
    }
    g_particles[index] = p;
}