#include "feature/shading.hlsli" // shading utils
#include "feature/shadow_mapping.hlsli" // shadow mapping
#include "feature/g_buffer_read.hlsli" // texture input
#include "common/buffer_camera.hlsli" // view projection matrix
#include "vertex_out/vertex_out_fullscreen.hlsli"

SamplerComparisonState shadow_sampler : register(s1);

Texture2D shadow_texture : register(t4);
Texture2D ramp_texture : register(t5);

cbuffer BufferLighting : register(b2)
{
    LightScene g_lights;
};

cbuffer BufferShadowMapping : register(b3)
{
    LightShadowScene g_light_shadows;
};

struct PixelOut
{
    float4 color : SV_Target0;
    float4 color_light : SV_Target1;
};

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    
    SurfaceData surface_w = GetSurfaceDataWorldFromGBuffer(pixel_in.uv, g_matrix_view_proj_inverse);

    float4 color = 0.0f;
    
    // shadow mapping
    float4 position_light = mul(surface_w.position, g_light_shadows.dir_light_shadow.matrix_light_view_proj);
    float3 dir_light_pos = position_light.xyz / position_light.w;
    dir_light_pos.x = dir_light_pos.x * 0.5f + 0.5f;
    dir_light_pos.y = dir_light_pos.y * -0.5f + 0.5f;
    float percent_lit = ComputeShadow(shadow_texture, shadow_sampler,
        position_light, surface_w.normal, g_lights.dir_light.direction_w);
    
    uint shading_model = surface_w.shading_model;
    if (shading_model == 0)
    {
        color.rgb = ComputeShadingBlinnPhong(surface_w, g_lights, g_view_position_w);
        color.w = surface_w.albedo.w;
        //color.rgb = ComputeShadingToon(ramp_texture, g_sampler_buffer, surface_w, g_lights, g_view_position_w);
        //color.w = surface_w.albedo.w;
    }
    else if (shading_model == 1)
    {
        // unlit
        color.rgb = 0.0f;
    }
    else if (shading_model == 2)
    {
        color.rgb = 1.0f;//ComputeShadingToon(ramp_texture, g_sampler_buffer, surface_w, g_lights, g_view_position_w);
        color.w = surface_w.albedo.w;
    }
    else if (shading_model == 3)
    {
        color.rgb = surface_w.albedo.rgb;
        color.w = surface_w.albedo.w;
    }
    else if (shading_model > 3)
    {
        color.rgb = float3(1.0f, 0.0f, 1.0f) * shading_model;
    }
    color.w = surface_w.albedo.w;
    pixel_out.color = color;
    
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0 || shading_model == 1)
    {
        pixel_out.color_light = color;
    }
    else
    {
        pixel_out.color_light = float4(0.0, 0.0, 0.0, 1.0);
    }
    return pixel_out;
}