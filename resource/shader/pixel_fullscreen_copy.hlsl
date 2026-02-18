#include "vertex_out/vertex_out_fullscreen.hlsli"

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(VertexOut pixelIn) : SV_TARGET
{
    float4 tex_color = tex.Sample(samplerState, pixelIn.uv);
    return tex_color;
}