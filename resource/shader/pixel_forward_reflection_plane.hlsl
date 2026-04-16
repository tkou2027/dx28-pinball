#include "vertex_out/vertex_out_base.hlsli"
#include "common/buffer_camera.hlsli" // view matrix
#include "feature/position_from_depth.hlsli"

struct PixelOut
{
    float4 color : SV_Target0; // color
};

SamplerState g_sampler : register(s0);
Texture2D g_texture_normal : register(t0);
Texture2D g_texture_material : register(t1);
Texture2D g_texture_depth : register(t2);
Texture2D g_texture_reflection : register(t3);

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    
    float2 screen_uv = pixel_in.uv; // TODO
    
    // material
    float4 materail = g_texture_material.Sample(g_sampler, screen_uv);
    float specular = materail.y;
    float roughness = materail.z;

    if (specular < 0.4 || specular > 0.6)
    {
        pixel_out.color = float4(0.0, 0.0, 0.0, 0.0);
        return pixel_out;
    }
    // normal
    float3 normal_w = g_texture_normal.Sample(g_sampler, screen_uv).xyz;
    float2 distortion = normal_w.xz * 0.5; // TODO: distortion strength 
    float2 reflect_uv = screen_uv + distortion;
    float3 reflection_color = g_texture_reflection.Sample(g_sampler, reflect_uv).rgb;
    
    screen_uv = saturate(screen_uv);
    // position
    float depth = g_texture_depth.Sample(g_sampler, screen_uv).r;
    float4 position_w = GetPositionFromDepth(screen_uv, depth, g_matrix_view_proj_inverse);
    // view
    float3 view_dir = normalize(g_view_position_w - position_w.xyz);
    
    float n_dot_v = saturate(dot(normal_w, view_dir));
    float fresnel = pow(1.0 - n_dot_v, 5.0f);

    reflection_color *= lerp(0.04, 1.0, fresnel);
    reflection_color *= 0.2f;

    pixel_out.color = float4(reflection_color, 1.0f);

    return pixel_out;
}