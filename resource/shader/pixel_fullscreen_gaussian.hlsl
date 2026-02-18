#include "vertex_out/vertex_out_fullscreen.hlsli"
Texture2D g_texture : register(t0);
SamplerState g_tex_sampler : register(s0);

cbuffer GaussianConfig : register(b0)
{
    float sample_offset;
    bool horizontal;
    float _padding_1;
    float _padding_2;
};

struct PixelOut
{
    float4 color : SV_Target0;
};

static const float g_weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;

    float2 uv_offset;
    if (horizontal)
    {
        uv_offset = float2(sample_offset, 0.0f);
    }
    else
    {
        uv_offset = float2(0.0f, sample_offset);
    }
    
    float3 result = g_texture.Sample(g_tex_sampler, pixel_in.uv).rgb * g_weights[0];
    [unroll]
    for (int i = 1; i < 5; ++i)
    {
        result += g_texture.Sample(g_tex_sampler, pixel_in.uv + uv_offset * i).rgb * g_weights[i];
        result += g_texture.Sample(g_tex_sampler, pixel_in.uv - uv_offset * i).rgb * g_weights[i];
    }
    pixel_out.color = float4(result, 1.0f);
    
    return pixel_out;
}