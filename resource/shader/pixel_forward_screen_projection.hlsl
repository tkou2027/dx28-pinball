#include "vertex_out/vertex_out_base.hlsli"

cbuffer MaterialForwardProjector : register(b0)
{
    // projector space
    float4x4 g_projector_matrix_view_proj;
    float3 g_position_w_top_left;
    float _padding_position_w_top_left;
    float3 g_position_w_bottom_right;
    float _padding_position_w_bottom_right;
};

struct PixelOut
{
    float4 color : SV_Target0; // color
};

SamplerState g_sampler : register(s0);
Texture2D g_texture_projector : register(t0);

float4 GetPositionFromUV(float2 uv)
{
    // assume vertical plane
    float2 position_xz = lerp(g_position_w_top_left.xz, g_position_w_bottom_right.xz, uv.x);
    float y = lerp(g_position_w_top_left.y, g_position_w_bottom_right.y, uv.y);
    return float4(position_xz.x, y, position_xz.y, 1.0);
}

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    
    float2 screen_uv = pixel_in.uv;
    float4 position_world = GetPositionFromUV(screen_uv);

    float4 position_proj = mul(position_world, g_projector_matrix_view_proj);
    clip(position_proj.w - 0.001);
    position_proj /= position_proj.w;
    
    // pixel_out.color = float4(pixel_in.uv, 0.0, 1.0);

    clip(position_proj.z);
    clip(1 - position_proj.z);
    
    float2 uv_proj = position_proj.xy * 0.5f + 0.5f;
    uv_proj.y = 1.0 - uv_proj.y;
    clip(uv_proj.x);
    clip(1 - uv_proj.x);
    clip(uv_proj.y);
    clip(1 - uv_proj.y);
    
    float4 color_projector = g_texture_projector.Sample(g_sampler, uv_proj);
    clip(color_projector.a - 0.001);
    pixel_out.color = color_projector;

    return pixel_out;
}