#include "../common/buffer_mesh.hlsli"
#include "../common/mesh_instance.hlsli"

struct VertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 color : COLOR;
    uint instance_id : SV_InstanceID;
};

float4 GetPositionLocal(in VertexIn vertex_in)
{
    return float4(vertex_in.position, 1.0);
}

float3 GetNormalLocal(in VertexIn vertex_in)
{
    return vertex_in.normal;
}

float4 GetTangentLocal(in VertexIn vertex_in)
{
    return vertex_in.tangent;
}

MeshInstance GetMeshInstanceData(uint instance_id)
{
    MeshInstance instance;
    instance.matrix_model = g_matrix_model;
    instance.matrix_model_inverse_transpose = g_matrix_model_inverse_transpose;
    instance.uv_offset = g_uv_offset;
    instance.uv_size = g_uv_size;
    return instance;
}