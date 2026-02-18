#include "vertex_out/vertex_out_fullscreen.hlsli"
Texture2D g_texture : register(t0);
SamplerState g_sampler_linear_clamp : register(s0);

cbuffer UpSampleConfig : register(b0)
{
    float g_filter_radius;
    float _padding_1;
    float _padding_2;
    float _padding_3;
};

float4 main(VertexOut pixel_in) : SV_TARGET
{
    // The filter kernel is applied with a radius, specified in texture
    // coordinates, so that the radius will vary across mip resolutions.
    float x = g_filter_radius;
    float y = g_filter_radius;
    float2 uv = pixel_in.uv;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    float3 a = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-x, y), 0).rgb;
    float3 b = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(0, y), 0).rgb;
    float3 c = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(x, y), 0).rgb;

    float3 d = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-x, 0), 0).rgb;
    float3 e = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(0, 0), 0).rgb;
    float3 f = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(x, 0), 0).rgb;

    float3 g = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-x, -y), 0).rgb;
    float3 h = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(0, -y), 0).rgb;
    float3 i = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(x, -y), 0).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    float3 upsample = e * 4.0f;
    upsample += (b + d + f + h) * 2.0f;
    upsample += (a + c + g + i);
    upsample *= 1.0f / 16.0f;

    return float4(upsample, 1.0f);
}