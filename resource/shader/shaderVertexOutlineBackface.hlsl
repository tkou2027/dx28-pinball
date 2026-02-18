#include "common.hlsli"
#include "common_outline.hlsli"

VertexOutOutline main(VertexInBase vertexIn)
{
    VertexOutOutline vertexOut;

    float4x4 mvp = mul(matrix_model, matrix_view_proj);
    float4 position_clip = mul(float4(vertexIn.position, 1.0), mvp);
    // normal to clip
    float4 normal_world = float4(mul(vertexIn.normal, (float3x3)matrix_model_inverse_transpose), 0.0f);
    float2 normal_clip = normalize(mul(normal_world, matrix_view_proj));
    // add thickness
    position_clip.xy += normal_clip * (material_outline.width.x * position_clip.w);
    vertexOut.position = position_clip;
    return vertexOut;
}