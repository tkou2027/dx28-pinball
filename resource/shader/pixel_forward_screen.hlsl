#include "vertex_out/vertex_out_base.hlsli"

struct PixelOut
{
    float4 color : SV_Target0; // color
    float4 emission : SV_Target1; // emission
};

// textures
Texture2D g_texture_diffuse : register(t0);
Texture2D g_texture_emission : register(t1);
Texture2D g_texture_screen_pixel : register(t2);
SamplerState g_sampler_linear : register(s0);
SamplerState g_sampler_point : register(s1);

// materails
cbuffer MaterialScreen : register(b0)
{
    // color
    float3 g_emission_color;
    float g_emission_intensity;
    // size
    float2 g_screen_pixels_scale;
    float _padding_0;
    float _padding_1;
};

float ComputeMipLevel(float2 uv)
{
    float2 duvdx = ddx(uv);
    float2 duvdy = ddy(uv);

    float scale = max(dot(duvdx, duvdx), dot(duvdy, duvdy));
    return 0.5f * log2(scale);
}

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;

    // base color
    float4 albedo = g_texture_diffuse.Sample(g_sampler_point, pixel_in.uv);
    clip(albedo.a - 0.1);
    // if (g_shading_model == 3)
    
    uint width, height;
    g_texture_diffuse.GetDimensions(width, height);
    float2 screen_size = float2(width, height);
    float2 pixel_uv = pixel_in.uv * screen_size * g_screen_pixels_scale;
    float4 screen_pixel = g_texture_screen_pixel.Sample(g_sampler_linear, pixel_uv);
    
    float mip_level = ComputeMipLevel(pixel_uv * 48.0f);
    float pixel_fade = saturate((mip_level - 1.0) / (4.0 - 1.0));

    // albedo.rgb = albedo.rgb * screen_pixel.rgb * 4.0f;
    float3 lcdColor = albedo.rgb * screen_pixel.rgb * 4.0;
    albedo.rgb = lerp(lcdColor, albedo.rgb, pixel_fade);
    pixel_out.color = albedo * 1.0f;

    //pixel_out.color = pixel_fade; //albedo;

    // TODO: calc emission
    float4 emission = g_texture_emission.Sample(g_sampler_linear, pixel_in.uv); // blurred emission
    emission.rgb = lerp(lcdColor, albedo.rgb, pixel_fade);
    emission.rgb = emission.rgb * g_emission_color * g_emission_intensity; // TODO: calc emission
    pixel_out.emission = albedo * 2.0f;

    return pixel_out;
}