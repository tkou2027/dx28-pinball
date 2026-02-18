#ifdef USE_VERTEX_IN_MESH_INSTANCED
#include "vertex_in/vertex_in_mesh_instanced.hlsli"
#elif defined USE_VERTEX_IN_MESH_SKINNED
#include "vertex_in/vertex_in_mesh_skinned.hlsli"
#else
#include "vertex_in/vertex_in_mesh_static.hlsli"
#endif

#include "vertex_out/vertex_out_base.hlsli"

float4 GetPositionLocal(in VertexIn vertex_in);

VertexOut main(VertexIn vertex_in)
{
    VertexOut vertex_out;
    
    // per mesh instance data
    MeshInstance instance = GetMeshInstanceData(vertex_in.instance_id);

    // local
    float4 position_l = GetPositionLocal(vertex_in);

    vertex_out.uv = vertex_in.uv * instance.uv_size + instance.uv_offset; // uv animation
    vertex_out.position_w = mul(position_l, instance.matrix_model);
    // vertex_out.position_w = floor(vertex_out.position_w * 16.0f) / 16.0f;
    float3x3 dir_vector_matrix = (float3x3) instance.matrix_model_inverse_transpose;
    vertex_out.normal_w = mul(vertex_in.normal, dir_vector_matrix);
    vertex_out.tangent_w = float4(mul(vertex_in.tangent.xyz, dir_vector_matrix), vertex_in.tangent.w);

    vertex_out.color = vertex_in.color;

    vertex_out.position_h = mul(vertex_out.position_w, g_matrix_view_proj);
    vertex_out.position_v = mul(vertex_out.position_w, g_matrix_view);
    
    // instance id
    vertex_out.instance_id = vertex_in.instance_id;

    return vertex_out;
}