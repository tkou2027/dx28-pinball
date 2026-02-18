#include "vertex_out/vertex_out_fullscreen.hlsli"

Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);
SamplerState samplerState : register(s0);

float4 main(VertexOut pixel_in) : SV_TARGET
{
    float4 tex_color = tex0.Sample(samplerState, pixel_in.uv);
    tex_color += tex1.Sample(samplerState, pixel_in.uv) * 0.1f;
    return tex_color;
}