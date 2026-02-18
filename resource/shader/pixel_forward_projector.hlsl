#include "vertex_out/vertex_out_base.hlsli"
#include "common/buffer_camera.hlsli" // view matrix
#include "feature/position_from_depth.hlsli"

cbuffer MaterialForwardProjector : register(b2)
{
    // projector space
    float4x4 g_projector_matrix_view_proj;
    float3 g_projector_position_w;
    float _padding_projector_position_w;
};

struct PixelOut
{
    float4 color : SV_Target0; // color
};

SamplerState g_sampler : register(s0);
Texture2D g_texture_normal : register(t0);
Texture2D g_texture_depth : register(t1);
Texture2D g_texture_projector : register(t2);

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    
    float2 screen_uv = pixel_in.uv; // TODO
    float depth = g_texture_depth.Sample(g_sampler, screen_uv).r;
    float4 position_world = GetPositionFromDepth(screen_uv, depth, g_matrix_view_proj_inverse);
    
    // pixel_out.color = float4(position_world.xyz * 0.05f, 1.0f);
    // return pixel_out;
    
    float4 position_proj = mul(position_world, g_projector_matrix_view_proj);
    clip(position_proj.w - 0.001);
    position_proj /= position_proj.w;
    
    clip(position_proj.z);
    clip(1 - position_proj.z);
    
    float2 uv_proj = position_proj.xy * 0.5f + 0.5f;
    clip(uv_proj.x);
    clip(1 - uv_proj.x);
    clip(uv_proj.y);
    clip(1 - uv_proj.y);
    
    float3 to_projector = normalize(g_projector_position_w - position_world.xyz);
    float3 normal = g_texture_normal.Sample(g_sampler, screen_uv).xyz;
    float facing = dot(normal, to_projector);

    clip(facing - 0.1f);
    // visualize uv
    // pixel_out.color = float4(uv_proj, 0.0f, 1.0f);
    float4 color_projector = g_texture_projector.Sample(g_sampler, uv_proj);
    clip(color_projector.a - 0.001);
    color_projector *= 0.5f; // TODO: intensity
    pixel_out.color = color_projector;

    return pixel_out;
}