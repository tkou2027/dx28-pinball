#include "vertex_out/vertex_out_fullscreen.hlsli"
#include "common/buffer_camera.hlsli" // view projection matrix
#include "feature/g_buffer_read.hlsli" // texture input

cbuffer BufferSSRConfig : register(b2)
{
    float3 g_color;
    float _padding_1;
    float g_min_distance;
    float g_max_distance;
    float _padding_2;
    float _padding_3;
};

SurfaceData GetSurfaceDataAt(float2 uv)
{
    SurfaceData surface_v = GetSurfaceDataViewFromGBuffer(uv, g_matrix_proj_inverse, g_matrix_view);
    return surface_v;
}

float GetDepthAt(float2 uv)
{
    // SurfaceData s = GetSurfaceDataAt(uv);
    // return s.position.z; //GetDepthFromGBuffer(uv);
    float depth = g_texture_buffer_depth.Sample(g_sampler_buffer, uv).r;
    float4 position = GetPositionFromDepth(uv, depth, g_matrix_proj_inverse);
    return position.z;
}

void GetScreenSize(out uint width, out uint height)
{
    g_texture_buffer_a.GetDimensions(width, height);
}

// ssr utils ====

struct PixelOut
{
    float4 color : SV_Target0;
};

PixelOut main(VertexOut input) : SV_Target
{
    PixelOut pixel_out;

    pixel_out.color = float4(g_color, 1.0f);
    
    return pixel_out;
}
