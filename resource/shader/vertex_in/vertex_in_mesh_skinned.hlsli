#include "../feature/skeletal_animation.hlsli"
#include "../common/buffer_mesh.hlsli"
#include "../common/mesh_instance.hlsli"

struct VertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 color : COLOR;
    int4 bone_index : BLENDINDICES;
    float4 bone_weight : BLENDWEIGHT;
    uint instance_id : SV_InstanceID;
};

StructuredBuffer<float4x4> g_bone_matrices : register(t0);

float4 GetPositionLocal(in VertexIn vertex_in)
{
    float4 position_l = float4(vertex_in.position, 1.0);
    position_l = GetPositionSkinned(position_l,
        vertex_in.bone_index, vertex_in.bone_weight, g_bone_matrices);
    return position_l;
}

float3 GetNormalLocal(in VertexIn vertex_in)
{
    float3 normal_l = GetNormalSkinned(vertex_in.normal,
        vertex_in.bone_index, vertex_in.bone_weight, g_bone_matrices);
    return normal_l;
}

float4 GetTangentLocal(in VertexIn vertex_in)
{
    float3 tangent_l = GetNormalSkinned(vertex_in.tangent.xyz,
        vertex_in.bone_index, vertex_in.bone_weight, g_bone_matrices);
    return float4(tangent_l, vertex_in.tangent.w);
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