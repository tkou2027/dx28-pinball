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
    
    float3 ray_origin = g_view_position_w;
    float3 ray_end = position_world.xyz;
    float3 ray_vec = ray_end - ray_origin;
    float ray_length = length(ray_vec);
    if (ray_length < 1e-4)
    {
        pixel_out.color = float4(0, 0, 0, 0);
        return pixel_out;
    }
    float3 ray_dir = ray_vec / ray_length;
    const int STEP_COUNT = 16;
    float step_size = ray_length / STEP_COUNT;

    float3 accum = 0;
    float transmittance = 1.0;

    float noise = frac(sin(dot(screen_uv, float2(12.9898, 78.233))) * 43758.5453);
    float3 pos = ray_origin + ray_dir * (noise * step_size);
    // float3 pos = ray_origin;
     // ====== Ray March ======
    [loop]
    for (int i = 0; i < STEP_COUNT; i++)
    {
        pos += ray_dir * step_size;
        float4 pos_proj = mul(float4(pos, 1), g_projector_matrix_view_proj);
        if (pos_proj.w <= 0)
        {
            continue;
        }
        pos_proj /= pos_proj.w;
        if (pos_proj.z < 0 || pos_proj.z > 1)
        {
            continue;
        }
        float2 uv_proj = pos_proj.xy * 0.5 + 0.5;

        if (any(uv_proj < 0) || any(uv_proj > 1))
        {
            continue;
        }
        // ---- gobo ----
        float3 gobo = g_texture_projector.Sample(g_sampler, uv_proj);
        // TODO: noise
        float density = 0.01;
        float3 light = gobo * density;
        accum += transmittance * light * step_size;
        // Beer-Lambert
        transmittance *= exp(-density * step_size);
        if (transmittance < 0.01)
        {
            break;
        }
    }
    pixel_out.color = float4(accum, 1.0);
    return pixel_out;

    return pixel_out;
}