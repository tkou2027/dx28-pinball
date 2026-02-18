#include "common.hlsli"
#include "common_mask.hlsli"

VertexOutMask main(VertexInBase vertexIn)
{
    VertexOutMask vertexOut;

    float4 position = float4(vertexIn.position, 1.0);
    float4x4 mvp = mul(matrix_model, matrix_view_proj);
    vertexOut.position = mul(position, mvp);

    return vertexOut;
}