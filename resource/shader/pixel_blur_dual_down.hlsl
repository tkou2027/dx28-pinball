#include "vertex_out/vertex_out_fullscreen.hlsli"
Texture2D g_texture : register(t0);
SamplerState g_sampler_linear_clamp : register(s0);

cbuffer DownSampleConfig : register(b0)
{
    float2 g_src_resolution;
    float _padding_1;
    float _padding_2;
};

float Luma(float3 color)
{
    return dot(color, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 KarisAverage(float3 color)
{
    return color / (1.0f + Luma(color));
}

float4 main(VertexOut pixel_in) : SV_TARGET
{
    float2 src_texel_size = 1.0f / g_src_resolution;
    float x = src_texel_size.x;
    float y = src_texel_size.y;
    float2 uv = pixel_in.uv;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    float3 a = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-2 * x, 2 * y), 0).rgb;
    float3 b = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(0, 2 * y), 0).rgb;
    float3 c = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(2 * x, 2 * y), 0).rgb;

    float3 d = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-2 * x, 0), 0).rgb;
    float3 e = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(0, 0), 0).rgb;
    float3 f = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(2 * x, 0), 0).rgb;

    float3 g = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-2 * x, -2 * y), 0).rgb;
    float3 h = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(0, -2 * y), 0).rgb;
    float3 i = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(2 * x, -2 * y), 0).rgb;

    float3 j = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-x, y), 0).rgb;
    float3 k = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(x, y), 0).rgb;
    float3 l = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(-x, -y), 0).rgb;
    float3 m = g_texture.SampleLevel(g_sampler_linear_clamp, uv + float2(x, -y), 0).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    float3 downsample;
    downsample = e * 0.125f;
    downsample += (a + c + g + i) * 0.03125f;
    downsample += (b + d + f + h) * 0.0625f;
    downsample += (j + k + l + m) * 0.125f;

    float4 color = float4(max(downsample, 0.0001f), 1.0f);

    return color;
}