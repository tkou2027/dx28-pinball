#include "vertex_out/vertex_out_fullscreen.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 main(VertexOut pixel_in) : SV_Target
{
    uint width, height;
    g_texture.GetDimensions(width, height);
    float2 texSize = float2(width, height);

    float blurRadius = 3;
    float separation = 0.8;

    if (blurRadius <= 0)
    {
        return g_texture.Sample(g_sampler, pixel_in.uv);
    }

    float4 colorSum = 0.0;
    float count = 0.0;

    // Box blur
    [loop]
    for (int y = -blurRadius; y <= blurRadius; ++y)
    {
        [loop]
        for (int x = -blurRadius; x <= blurRadius; ++x)
        {
            float2 offset = float2(x, y) * separation;
            float2 uv = (pixel_in.postion_h.xy + offset) / texSize;

            colorSum += g_texture.Sample(g_sampler, uv);
            count += 1.0;
        }
    }

    float4 result = colorSum / count;
    return result;
}
