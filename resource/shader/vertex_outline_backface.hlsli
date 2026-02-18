#ifdef USE_VERTEX_IN_MESH_SKINNED
#include "vertex_in/vertex_in_mesh_skinned.hlsli"
#else
#include "vertex_in/vertex_in_mesh_static.hlsli"
#endif

#include "common/common_buffer_mesh.hlsli"
#include "common_outline.hlsli"

#define USE_VERTEX_OUT_POS
#include "common/common_vertex_out.hlsli"

float4 GetPositionLocal(in VertexIn vertex_in);

VertexOut main(VertexIn vertex_in)
{
    VertexOut vertex_out;

    // local
    float4 position_l = GetPositionLocal(vertex_in);
    float3 normal_l = GetNormalLocal(vertex_in);

    float4x4 mvp = mul(matrix_model, matrix_view_proj);
    float4 postion_h = mul(position_l, mvp);
    
    float4 normal_w = float4(mul(normal_l, (float3x3) matrix_model_inverse_transpose), 0.0f);
    float4 normal_h = normalize(mul(normal_w, matrix_view_proj));

    // add thickness
    postion_h.xy += normal_h * (material_outline.width.x * normal_h.w);
    vertex_out.position_h = postion_h;

    return vertex_out;
}