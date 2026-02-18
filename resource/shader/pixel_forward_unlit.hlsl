#include "vertex_out/vertex_out_base.hlsli"

struct PixelOut
{
    float4 color : SV_Target0; // color
    float4 emission : SV_Target1; // emission
};

// textures
Texture2D g_texture_diffuse : register(t0);
Texture2D g_texture_emission : register(t1);
SamplerState g_sampler_texture : register(s0);

// materails
cbuffer MaterialDefault : register(b0)
{
    float g_metallic;
    float g_specular;
    float g_roughness;
    uint g_shading_model;
    float3 g_emission_color;
    float g_emission_intensity;
};

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;

    // base color
    float4 albedo = g_texture_diffuse.Sample(g_sampler_texture, pixel_in.uv);
    clip(albedo.a - 0.1);
    // if (g_shading_model == 3)

    pixel_out.color = albedo;

    // TODO: calc emission
    float4 emission = g_texture_emission.Sample(g_sampler_texture, pixel_in.uv);
    emission.rgb = emission.rgb * g_emission_color * g_emission_intensity; // TODO: calc emission
    pixel_out.emission = emission;

    return pixel_out;
}