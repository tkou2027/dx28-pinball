#include "texture_particle.hlsli"
#include "../common/buffer_camera.hlsli"

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

// float2 cornerUV[6] =
// {
//     float2(0.0, 1.0),
//     float2(1.0, 1.0),
//     float2(0.0, 0.0),
//     float2(0.0, 0.0),
//     float2(1.0, 1.0),
//     float2(1.0, 0.0)
// };

// VSOut main(uint vertexId : SV_VertexID)
// {
//     VSOut o;
// 
//     uint particleIndex = vertexId / 6;
//     uint cornerIndex = vertexId % 6;
// 
//     // read particle
//     TextureParticle p = g_particles[particleIndex];
// 
//     float2 c = cornerPos[cornerIndex];
//     
//     // float3 right = float3(g_matrix_view[0][0], g_matrix_view[1][0], g_matrix_view[2][0]);
//     // float3 up = float3(g_matrix_view[0][1], g_matrix_view[1][1], g_matrix_view[2][1]);
//     // float3 worldPos = p.position + (right * c.x + up * c.y) * 1.0;
//     
//     float3 worldPos = position;
// 
//     o.position = mul(float4(worldPos, 1.0), g_matrix_view_proj); // matrix_view_proj from common.hlsli b0
//     o.uv = p.uv; //cornerUV[cornerIndex];
//    // o.life = p.life;
// 
//     return o;
// }
VSOut main(VSIn vin, uint instanceID : SV_InstanceID, uint vertexId : SV_VertexID)
{
    VSOut o;
    TextureParticle p = g_particles[instanceID];

    float3 right = float3(g_matrix_view[0][0], g_matrix_view[1][0], g_matrix_view[2][0]);
    float3 up = float3(g_matrix_view[0][1], g_matrix_view[1][1], g_matrix_view[2][1]);
    float3 worldPos = p.position + (right * vin.position.x + up * vin.position.y) * 0.5f;//a * saturate(p.life);
    o.position = mul(float4(worldPos, 1.0), g_matrix_view_proj);
    o.uv = p.uv;
    o.uv_local = vin.uv_local;
    o.life = p.life;
    return o;
}