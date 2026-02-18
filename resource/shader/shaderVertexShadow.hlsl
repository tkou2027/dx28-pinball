#include "common.hlsli"

struct VertexOut
{
    float4 position : SV_POSITION;
};

VertexOut main(VertexInBase vertexIn)
{
    VertexOut vertexOut;

    float4 position = float4(vertexIn.position, 1.0);
    vertexOut.position = mul(position, mul(matrix_model, matrix_directional_light_view_proj));

    return vertexOut;
}