#include "common/buffer_camera.hlsli" // view projection matrix
#include "vertex_out/vertex_out_base.hlsli"
#include "feature/shading.hlsli" // shading utils

struct PixelOut
{
    float4 color : SV_Target0; // color
};

// textures
Texture2D g_texture_diffuse : register(t0);
Texture2D g_texture_emission : register(t1);
TextureCube g_texture_environment : register(t2);
SamplerState g_sampler_texture : register(s0);

float3 g_light_dir = float3(0.0f, -0.5f, 0.83f);

// materails
PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;

    float3 normal = normalize(pixel_in.normal_w);
    float3 light_dir = normalize(g_light_dir);
    float3 view_dir = normalize(g_view_position_w - pixel_in.position_w.xyz);

    float3 reflect_dir = reflect(-view_dir, normal);
    float3 reflect_color = g_texture_environment.Sample(g_sampler_texture, -reflect_dir); // hack

    float specular = ComputeSpecular(normal, light_dir, view_dir, 16.0);

    float rim = ComputeRim(normal, view_dir, 4.0);
    
    float3 specular_color = float3(0.1f, 0.1f, 0.1f);
    float3 rim_color = float3(0.1f, 0.1f, 0.1f);
    
    pixel_out.color = float4(specular_color, 1.0) * specular
        + float4(specular_color, 1.0) * rim
        + float4(reflect_color, 1.0) * 0.5;
    
    // pixel_out.color = float4(reflect_color, 1.0) * 0.1f + 0.02f;

    return pixel_out;
}