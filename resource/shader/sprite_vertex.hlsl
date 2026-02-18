// 定数バッファ
cbuffer BufferPerProjection : register(b0)
{
    float4x4 g_matrix_proj;
};

cbuffer BufferPerSprite : register(b1)
{
    float4x4 g_matrix_model;
};

#include "vertex_out/vertex_out_sprite.hlsli"

// 頂点シェーダ
VertexOut main(
    in float4 position: POSITION0,
    in float2 texcoord: TEXCOORD0,
    in float4 color: COLOR)
{
    VertexOut vertex_out;
    float4x4 mtx = mul(g_matrix_model, g_matrix_proj);
    vertex_out.uv = texcoord;
    vertex_out.color = color;
    vertex_out.position_h =  mul(position, mtx);
    return vertex_out;
}