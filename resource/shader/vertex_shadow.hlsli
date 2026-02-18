#ifdef USE_VERTEX_IN_MESH_SKINNED
#include "vertex_in/vertex_in_mesh_skinned.hlsli"
#else
#include "vertex_in/vertex_in_mesh_static.hlsli"
#endif
#include "feature/shadow_mapping.hlsli"
// constant buffer
#include "common/buffer_mesh.hlsli"
// cbuffer BufferBufferShadowMappingPerLight : register(b3)
// {
//     float4x4 g_matrix_light_view_proj;
// };
cbuffer BufferShadowMapping : register(b3)
{
    LightShadowScene g_light_shadows;
};


struct VertexOut
{
    float4 position_h : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 GetPositionLocal(in VertexIn vertex_in);

VertexOut main(VertexIn vertex_in)
{
    VertexOut vertex_out;

    // local
    float4 position_l = GetPositionLocal(vertex_in);

    vertex_out.uv = vertex_in.uv;
    float4 position_w = mul(position_l, g_matrix_model);
    //vertex_out.position_h = mul(position_w, g_matrix_light_view_proj);
    vertex_out.position_h = mul(position_w, g_light_shadows.dir_light_shadow.matrix_light_view_proj);

    return vertex_out;
}