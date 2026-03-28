#include "vertex_out/vertex_out_fullscreen.hlsli"

cbuffer cbPreview : register(b0)
{
    int g_mode; // 0: NORMAL, 1: MATERIAL, 2: DEPTH, 3: SRGB
    float g_z_near;
    float g_z_far;
    float padding;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);



float4 main(VertexOut pixelIn) : SV_TARGET
{
    float4 tex_color = g_texture.Sample(g_sampler, pixelIn.uv);

    // 0: NORMAL (G-Buffer A)
    if (g_mode == 0)
    {
        return float4(tex_color.xyz * 0.5f + 0.5f, 1.0f);
    }
    // 1: MATERIAL (G-Buffer B)
    else if (g_mode == 1)
    {
        return float4(tex_color.rgb, 1.0f);
    }
    // 2: DEPTH
    else if (g_mode == 2)
    {
        float d = tex_color.r;
        float linearZ = (2.0f * g_z_near) / (g_z_far + g_z_near - d * (g_z_far - g_z_near));
        return float4(linearZ.xxx, 1.0f);
    }
    // 3: SRGB
    else if (g_mode == 3)
    {
        return float4(tex_color.rgb, 1.0f);
    }
    return tex_color;
}