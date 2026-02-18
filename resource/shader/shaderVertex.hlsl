#include "common.hlsli"
#include "common_cel.hlsli"

VertexOutCel main(VertexInBase vertexIn)
{
    VertexOutCel vertexOut;

    float4 position = float4(vertexIn.position, 1.0);
    
    float4x4 mvp = mul(matrix_model, matrix_view_proj);
    vertexOut.position = mul(position, mvp);
    vertexOut.uv = vertexIn.uv;
    vertexOut.position_world = mul(position, matrix_model);
    vertexOut.normal_world = mul(vertexIn.normal, (float3x3)matrix_model_inverse_transpose);
    vertexOut.position_dir_light = mul(vertexOut.position_world, matrix_directional_light_view_proj);
    return vertexOut;
}