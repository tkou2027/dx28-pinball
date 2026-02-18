#include "vertex_out/vertex_out_base.hlsli"

struct PixelOut
{
    float4 color : SV_Target0; // color
    float4 emission : SV_Target1; // emission
};

// textures
// screen basic
Texture2D g_texture_base : register(t0);
Texture2D g_texture_screen_pixel : register(t1);
// monitor effect
Texture2D g_texture_world : register(t2);

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

    float2 parallax = 0; // pixel_in.normal_w.xy * 0.05f;
    // sample background (and mask)
    float4 color_base = g_texture_base.Sample(g_sampler_point, pixel_in.uv);
    float world_mask = step(0.5, color_base.a);

    // sample world texuture
    float2 screen_uv = pixel_in.position_h.xy / float2(1600, 900); // TODO
    
    float3 color_world = g_texture_world.Sample(g_sampler_linear, screen_uv + parallax).rgb;

    // combine by mask
    float3 color_out = lerp(color_world, color_base.rgb, world_mask);
    
    // led effect
    float2 pixel_uv = pixel_in.uv * g_screen_pixels_scale;
    float4 screen_pixel = g_texture_screen_pixel.Sample(g_sampler_linear, pixel_uv);
    
    float mip_level = ComputeMipLevel(pixel_uv * 48.0f); // TODO
    float pixel_fade = saturate((mip_level - 1.0) / (4.0 - 1.0));

    float3 color_lcd = color_out * screen_pixel.rgb * 4.0;
    color_out = lerp(color_lcd, color_out, pixel_fade);
    pixel_out.color = float4(color_out, 1.0);

    //pixel_out.color = pixel_fade; //albedo;

    // TODO: calc emission
    pixel_out.emission = pixel_out.color;

    return pixel_out;
}