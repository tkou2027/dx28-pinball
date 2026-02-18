#include "vertex_out/vertex_out_base.hlsli"
#include "feature/shading.hlsli" 
// gbuffer
struct PixelOut
{
    float4 buffer_a : SV_Target0; // world normal
    float4 buffer_b : SV_Target1; // metallic, specular, roughness, shading model
    float4 buffer_c : SV_Target2; // base color
    float4 buffer_d : SV_Target3; // output hdr color, emmision
};

// textures
Texture2D g_texture_diffuse : register(t0);
Texture2D g_texture_emission : register(t1);
// pbr
Texture2D g_texture_normal : register(t2);
Texture2D g_texture_metallic : register(t3);
Texture2D g_texture_roughness : register(t4);
// ramp
Texture2D g_texture_ramp : register(t5);

// TextureCube g_texture_environment : register(t1);
SamplerState g_sampler_warp : register(s0);
SamplerState g_sampler_clamp : register(s1);

// materails
#include "common/buffer_camera.hlsli"
cbuffer MaterialDefault : register(b2)
{
    float4 g_base_color;

    float g_metallic;
    float g_specular;
    float g_roughness;
    uint g_shading_model;

    float3 g_emission_color;
    float g_emission_intensity;
};

cbuffer MaterialCel : register(b3)
{
    float3 g_light_dir_w;
    float g_ramp_offset;
}

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    // normal
    // no normal map
    pixel_out.buffer_a.xyz = normalize(pixel_in.normal_w);
    pixel_out.buffer_a.w = 0.0f;
    // material
    float metallic = g_metallic * g_texture_metallic.Sample(g_sampler_warp, pixel_in.uv);
    pixel_out.buffer_b.r = metallic;
    pixel_out.buffer_b.g = g_specular;
    pixel_out.buffer_b.b = g_roughness * g_texture_roughness.Sample(g_sampler_warp, pixel_in.uv);
    pixel_out.buffer_b.a = lerp(1.0, 0.0, step(g_metallic, 0.2)) / 16.0; //(float) g_shading_model / 16.0f; //1.0f;//asfloat(g_shading_model);
    
    // base color
    float4 albedo = g_base_color * g_texture_diffuse.Sample(g_sampler_warp, pixel_in.uv);
    clip(albedo.a - 0.1);
    // toon
    albedo.rgb = ComputeShadingToon(
        g_texture_ramp, g_sampler_clamp,
        pixel_in.position_w.xyz, pixel_in.normal_w,
        albedo, g_ramp_offset,
        g_light_dir_w, g_view_position_w
    );
    pixel_out.buffer_c = albedo;

    // TODO: calc emission
    //float4 emission = g_texture_emission.Sample(g_sampler_warp, pixel_in.uv);
    //emission.rgb = emission.rgb * g_emission_color * g_emission_intensity; // TODO: calc emission
    //pixel_out.buffer_d = emission;
    
    pixel_out.buffer_d = 0.0f;

    return pixel_out;
}