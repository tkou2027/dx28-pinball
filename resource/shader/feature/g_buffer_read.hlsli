#ifndef __G_BUFFER_READ__
#define __G_BUFFER_READ__
#include "shading.hlsli" // surface data
#include "position_from_depth.hlsli"

Texture2D g_texture_buffer_a : register(t0);
Texture2D g_texture_buffer_b : register(t1);
Texture2D g_texture_buffer_c : register(t2);
// depth stencil
Texture2D g_texture_buffer_depth : register(t3);

SamplerState g_sampler_buffer : register(s0);

float GetDepthFromGBuffer(float2 position_s)
{
    float depth = g_texture_buffer_depth.Sample(g_sampler_buffer, position_s).r;
    return depth;
}

SurfaceData GetSurfaceDataWorldFromGBuffer(float2 position_s, float4x4 view_proj_inverse)
{
    SurfaceData surface;
    // base color
    surface.albedo = g_texture_buffer_c.Sample(g_sampler_buffer, position_s);
    // material
    float4 materail = g_texture_buffer_b.Sample(g_sampler_buffer, position_s);
    surface.metallic = materail.x;
    surface.specular = materail.y;
    surface.roughness = materail.z;
    surface.shading_model = (uint)round(materail.w * 16.0f);
    // normal
    surface.normal = g_texture_buffer_a.Sample(g_sampler_buffer, position_s).xyz;
    // compute position form depth
    float depth = g_texture_buffer_depth.Sample(g_sampler_buffer, position_s).r;
    surface.position = GetPositionFromDepth(position_s, depth, view_proj_inverse);
    return surface;
}

SurfaceData GetSurfaceDataViewFromGBuffer(float2 position_s, float4x4 proj_inverse, float4x4 view)
{
    SurfaceData surface;
    // base color
    surface.albedo = g_texture_buffer_c.Sample(g_sampler_buffer, position_s);
    // material
    float4 materail = g_texture_buffer_b.Sample(g_sampler_buffer, position_s);
    surface.metallic = materail.x;
    surface.specular = materail.y;
    surface.roughness = materail.z;
    surface.shading_model = (uint)round(materail.w * 16.0f);;
    float3 normal_w = g_texture_buffer_a.Sample(g_sampler_buffer, position_s).xyz;
    surface.normal = mul(normal_w, (float3x3) view);
    // compute position form depth
    float depth = g_texture_buffer_depth.Sample(g_sampler_buffer, position_s).r;
    surface.position = GetPositionFromDepth(position_s, depth, proj_inverse);
    return surface;
}
#endif