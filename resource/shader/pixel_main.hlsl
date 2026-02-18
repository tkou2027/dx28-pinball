struct VertexOut
{
    float4 postion_h : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 position_w : TEXCOORD1;
    float3 normal_w : NORMAL;
    float4 position_lw : TEXCOORD2; // TODO
    float4 position_v : TEXCOORD3; // TODO
    float3 normal_v : TEXCOORD4; // TODO
};
#include "common/common_buffer_mesh.hlsli"

#include "common_cel.hlsli"

Texture2D g_texture_diffuse : register(t0);
TextureCube g_texture_env : register(t1);

SamplerState g_sampler_diffuse : register(s0);
SamplerState g_sampler_env : register(s1);

struct PixelOut
{
    float4 color : SV_Target0;
    float4 color_test : SV_Target1;
    
    float4 position_v : SV_Target2;
    float3 normal_v : SV_Target3;
};

float4 ComputeColor(VertexOut pixel_in)
{
    // dirs
    float3 normal = normalize(pixel_in.normal_w);
    float3 view_dir = normalize(camera_position - pixel_in.position_w.xyz);

    float3 view_reflect = reflect(-view_dir, normal);
    float3 reflect_color = g_texture_env.Sample(g_sampler_env, view_reflect);

    return float4(reflect_color * 0.8f, 1.0f);
}

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    
    float4 albedo = g_texture_diffuse.Sample(g_sampler_diffuse, pixel_in.uv);
    // float4 albedo = ComputeColor(pixel_in);
    
    pixel_out.color = albedo; // float4(1.0f, 1.0f, 1.0f, 1.0f);

     // pixel_out.color_test = float4(1.0f, 0.0f, 1.0f, 1.0f);
    float brightness = dot(albedo.rgb, float3(0.2126, 0.7152, 0.0722));
    if (brightness > 0.9)
    {
        pixel_out.color_test = albedo;
    }
    else
    {
        pixel_out.color_test = float4(0.0, 0.0, 0.0, 1.0);

    }
    pixel_out.position_v = pixel_in.position_v;
    pixel_out.normal_v = normalize(pixel_in.normal_v);

    return pixel_out;
}