#ifdef USE_VERTEX_IN_MESH_SKINNED
#include "vertex_in/vertex_in_mesh_skinned.hlsli"
#else
#include "vertex_in/vertex_in_mesh_static.hlsli"
#endif

#include "common/common_buffer_mesh.hlsli"
#include "common_cel.hlsli"

// #define USE_VERTEX_OUT_POS_TEX_LIGHTING
// #include "common/common_vertex_out.hlsli"
struct VertexOut
{
    float4 postion_h : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 position_w : TEXCOORD1;
    float3 normal_w : NORMAL;
    float4 position_lw : TEXCOORD2; // TODO
    float4 position_v : TEXCOORD3; // TODO
    float3 normal_v : TEXCOORD4; // TODO
};

float4 GetPositionLocal(in VertexIn vertex_in);

VertexOut main(VertexIn vertex_in)
{
    VertexOut vertex_out;

    // local
    float4 position_l = GetPositionLocal(vertex_in);

    // float4x4 mvp = mul(matrix_view_proj, matrix_model);
    // vertex_out.postion_h = mul(position_l, mvp);
    vertex_out.uv = vertex_in.uv;
    vertex_out.position_w = mul(position_l, matrix_model);
    vertex_out.normal_w = mul(vertex_in.normal, (float3x3)matrix_model_inverse_transpose);
    
    vertex_out.postion_h = mul(vertex_out.position_w, matrix_view_proj);
    
    vertex_out.position_v = mul(vertex_out.position_w, matrix_view);
    vertex_out.normal_v = mul(vertex_out.normal_w, (float3x3)matrix_view);

    vertex_out.position_lw = mul(vertex_out.position_w, matrix_directional_light_view_proj); // TODO
    
    return vertex_out;
}