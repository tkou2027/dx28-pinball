#include "texture_particle.hlsli"
static const int SHAPE_PLANE = 0;
static const int SHAPE_CYLINDER = 1;

RWStructuredBuffer<TextureParticle> g_particles : register(u0);

cbuffer ScreenParams : register(b0)
{
    float4x4 g_matrix_model;
    uint g_width;
    uint g_height;
    uint g_shape_type;
    float _padding_1;
};

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= g_width || id.y >= g_height)
        return;

    uint index = id.y * g_width + id.x;

    float2 uv = (float2(id.xy) + 0.5) / float2(g_width, g_height);
    uv.y = 1.0 - uv.y;

    float3 pos;
    // [branch]ÅH
    if (g_shape_type == SHAPE_PLANE)
    {
        pos.x = lerp(-0.5, 0.5, uv.x);
        pos.y = lerp(1.0, 0.0, uv.y);
        pos.z = 0.0;
    }
    else if (g_shape_type == SHAPE_CYLINDER)
    {
        float theta = -uv.x * 2.0 * 3.14159265;
        pos.x = sin(theta);
        pos.z = cos(theta);
        pos.y = lerp(-0.5f, 0.5f, uv.y);
    }
    
    float4 pos_w = mul(float4(pos, 1.0), g_matrix_model);

    TextureParticle p;
    p.position = pos_w.xyz;
    p.velocity = float3(0, 0, 0);
    p.uv = uv;
    p.life = 1e9;
    p.delay = 1.0f - uv.y;

    g_particles[index] = p;
}
