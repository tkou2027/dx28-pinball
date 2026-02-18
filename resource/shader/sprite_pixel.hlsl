
#include "vertex_out/vertex_out_sprite.hlsli"

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

// ピクセルシェーダー
float4 main(VertexOut pixel_in) : SV_Target
{
    float4 tex_color = tex.Sample(samplerState, pixel_in.uv) * pixel_in.color;
    clip(tex_color.a - 0.1f);
    return tex_color;
}