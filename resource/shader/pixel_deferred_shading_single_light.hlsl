#include "feature/shading.hlsli" // shading utils
#include "feature/shading_pbr.hlsli" // shading utils
#include "feature/g_buffer_read.hlsli" // texture input
#include "common/buffer_camera.hlsli" // view projection matrix
#include "vertex_out/vertex_out_base.hlsli"


cbuffer BufferScreenSize : register(b2)
{
    float g_screen_width;
    float g_screen_height;
    float _padding_screen_size_0;
    float _padding_screen_size_1;
};

cbuffer BufferLighting : register(b3)
{
    LightScene g_lights;
};

struct PixelOut
{
    float4 color : SV_Target0;
    float4 color_light : SV_Target1;
};

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    
    float2 screen_uv = pixel_in.position_h.xy / float2(1600, 900); // / pixel_in.position_h.w;
    // screen_uv = screen_uv * float2(0.5, -0.5) + 0.5f;
    SurfaceData surface_w = GetSurfaceDataWorldFromGBuffer(screen_uv, g_matrix_view_proj_inverse);

    float4 color = 0.0f;
    uint shading_model = surface_w.shading_model;
    if (shading_model == 0)
    {        
        LightPoint light = g_lights.point_light[pixel_in.instance_id];
        //float3 to_light = light.position_w - surface_w.position.xyz;
        //if (dot(to_light, to_light) > 100.0f) // TODO
        //{
        //    discard;
        //}
        
        float3 view_dir = normalize(g_view_position_w - surface_w.position.xyz);
        // color.rgb = ComputeShadingBlinnPhongPoint(surface_w, light, view_dir);
        color.rgb = ComputeShadingCookTorrancePointLight(surface_w, light, view_dir);
        // color.rgb = surface_w.normal;
        // color.rgb = surface_w.position.xyz;
        color.w = surface_w.albedo.w;
    }
    else
    {
        color = 0.0f;
    }
    
    pixel_out.color = color;
    return pixel_out;
}