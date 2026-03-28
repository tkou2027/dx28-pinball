#include "texture_particle.hlsli"
#include "../common/buffer_camera.hlsli"

cbuffer PlaneCommon : register(b2)
{
    float2 g_uv_offset;
    float2 g_uv_scale;
    float2 g_size;
    float _plane_common_padding_1;
    float _plane_common_padding_2;
};

// particle buffer
StructuredBuffer<TextureParticle> g_particles : register(t1);

struct VSIn
{
    float3 position : POSITION; // quad local pos (x,y,0)
    float2 uv_local : TEXCOORD0; // quad local pos (x,y,0)
};

struct VSOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 uv_local : TEXCOORD1;
    float life : TEXCOORD2;
};

float2 cornerPos[6] =
{
    float2(-0.5, -0.5),
    float2(0.5, -0.5),
    float2(-0.5, 0.5),
    float2(-0.5, 0.5),
    float2(0.5, -0.5),
    float2(0.5, 0.5)
};

VSOut main(VSIn vin, uint instanceID : SV_InstanceID, uint vertexId : SV_VertexID)
{
    VSOut o;
    TextureParticle p = g_particles[instanceID];

    // float3 right = float3(g_matrix_view[0][0], g_matrix_view[1][0], g_matrix_view[2][0]);
    // float3 up = float3(g_matrix_view[0][1], g_matrix_view[1][1], g_matrix_view[2][1]);
    float3 position_w = p.position + (p.right * vin.position.x * g_size.x + p.up * vin.position.y * g_size.y);
    o.position = mul(float4(position_w, 1.0), g_matrix_view_proj);
    o.uv = p.uv * g_uv_scale + g_uv_offset;
    o.uv_local = vin.uv_local;
    o.life = p.life;
    return o;
}