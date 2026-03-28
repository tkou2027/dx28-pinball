#include "texture_particle.hlsli"
#include "../common/buffer_camera.hlsli"

// particle buffer
StructuredBuffer<TextureParticle> g_particles : register(t1);

cbuffer ParticleConfig : register(b2)
{
    uint g_width;
    uint g_height;
    float _particle_config_padding_0;
    float _particle_config_padding_1;
};

struct VSOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float life : TEXCOORD1;
    float3 normal_w : NOTMAL0;
};

TextureParticle LoadParticle(int x, int y)
{
    x = clamp(x, 0, g_width - 1);
    y = clamp(y, 0, g_height - 1);
    return g_particles[y * g_width + x];
}

VSOut main(uint vertex_id : SV_VertexID)
{
    VSOut o;
    uint x = vertex_id % g_width;
    uint y = vertex_id / g_width;
    
    TextureParticle particle = LoadParticle(x, y);
    float3 pos = particle.position;

    float2 texel = float2(1.0 / g_width, 1.0 / g_height);

    // compute normal
    float3 pL = LoadParticle(x - 1, y).position;
    float3 pR = LoadParticle(x + 1, y).position;
    float3 pD = LoadParticle(x, y - 1).position;
    float3 pU = LoadParticle(x, y + 1).position;
    float3 dx = pR - pL;
    float3 dy = pU - pD;
    float3 normal = normalize(cross(dx, dy));

    o.position = mul(float4(pos, 1.0), g_matrix_view_proj);
    o.uv = particle.uv;
    o.life = particle.life;
    o.normal_w = normal;

    return o;
}