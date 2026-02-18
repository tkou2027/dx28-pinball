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

// TODO
float hash(uint n)
{
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return float(n & 0x7fffffffU) / float(0x7fffffff);
}

float noise(float2 p)
{
    float color = g_texture_noise.SampleLevel(
        g_sampler,
        p,
        0
    );
    color = color * 2.0f - 1.0f;
    return color;
}

float2 curl2D(float2 p)
{
    float eps = 0.1;

    float n1 = noise(p + float2(0, eps));
    float n2 = noise(p - float2(0, eps));
    float n3 = noise(p + float2(eps, 0));
    float n4 = noise(p - float2(eps, 0));

    float dx = n1 - n2;
    float dy = n3 - n4;

    return normalize(float2(dx, -dy));
}

// [numthreads(256, 1, 1)]
// void main(uint3 id : SV_DispatchThreadID)
// {
//     uint index = id.x;
//     if (index >= g_total_particles)
//         return;
// 
//     TextureParticle p = g_particles[index];
// 
//     // initialize
//     if (p.life > 1000.0f)
//     {
//         float3 g_hit_pos = float3(0.0, 0.0, 0.0);
//         float g_hit_force = 10.0f;
//         float rnd = hash(index);
//         float3 dir = normalize(p.position - g_hit_pos);
//         p.velocity = dir * (g_hit_force + rnd * 10.0f);
//         //p.velocity.y += 20.0f + rnd * 30.0f;
//         p.life = 2.0f;
//     }
//     else if (p.life > 0.0f)
//     {
//         p.velocity += g_gravity * g_dt;
//         p.velocity *= 0.98;
// 
//         float2 curl = curl2D(
//            (p.position.xz + p.position.y * float2(1.7, 2.3)) * 0.5f);
//         float3 flowDir = float3(curl.x, -1.0, curl.y);
// 
//         p.velocity += flowDir * 80.0f * g_dt;
// 
//         if (p.position.y < -20.0)
//         {
//             p.velocity.y = -p.velocity.y * 0.4; // bounce
//             p.velocity.xz *= 0.7; // friction
//         }
//         
//         p.position += p.velocity * g_dt;
//         p.life -= g_dt;
//     }
//     g_particles[index] = p;
// }

[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint index = id.x;
    if (index >= g_total_particles)
        return;

    TextureParticle p = g_particles[index];

    // initialize
    if (p.life > 1000.0f)
    {
        if (p.delay > 0.0f)
        {
            p.delay -= g_dt;
        }
        else
        {
            float3 g_hit_pos = float3(0.0, 0.0, 0.0);
            float g_hit_force = 10.0f;
            float rnd = hash(index);
            float3 dir = -normalize(p.position - g_hit_pos);
            p.velocity = dir * (g_hit_force + rnd * 10.0f);
            //p.velocity.y += 20.0f + rnd * 30.0f;
            p.life = 5.0f;
        }

    }
    else if (p.life > 0.0f)
    {
        p.velocity += g_gravity * g_dt * 2.0f;
        p.velocity *= 0.98;
        
        
        float3 flow = normalize(float3(
            noise(p.uv.xy),
            -1.0,
            noise(p.uv.yx)
        ));
        
        p.velocity += flow * 20.0f * (1.0 - p.life * 0.5) * g_dt;
        
        
        if (p.position.y < -80.0)
        {
            p.velocity.y = -p.velocity.y * 0.4; // bounce
            p.velocity.xz *= 0.7; // friction
        }
        
        p.position += p.velocity * g_dt;
        p.life -= g_dt;
    }
    g_particles[index] = p;
}