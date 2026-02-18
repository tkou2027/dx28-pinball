#include "vertex_out/vertex_out_base.hlsli"
// depth pre pass (and normal)
struct PixelOut
{
    float4 buffer_a : SV_Target0; // world normal
};

Texture2D g_texture_diffuse : register(t0);
SamplerState g_sampler : register(s0);

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    // albedo for clipping
    float4 albedo = g_texture_diffuse.Sample(g_sampler, pixel_in.uv);
    clip(albedo.a - 0.1);
    // normal
    float3 normal_w = normalize(pixel_in.normal_w);
    pixel_out.buffer_a.xyz = normal_w;
    pixel_out.buffer_a.w = 0.0f;
    return pixel_out;
}