#include "../common/buffer_camera.hlsli"
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

StructuredBuffer<MeshInstance> g_instance_data_list : register(t0);

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
    return g_instance_data_list[instance_id];
}