#include "vertex_out/vertex_out_fullscreen.hlsli"
#include "common/buffer_camera.hlsli" // view projection matrix
#include "feature/g_buffer_read.hlsli" // texture input

Texture2D g_texture_color : register(t4);

cbuffer BufferSSRConfig : register(b2)
{
    float g_max_distance;
    float g_resolution;
    float g_thickness;
    int g_steps;
};

// cbuffer SSRConfig : register(b2)
// {
//     float4x4 projection_mat;
// };

// interface for sampling textuers ====
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

float GetReflectionMask(SurfaceData surface)
{
    return surface.specular * (surface.shading_model == 0);
}

float4 GetReflectionColor(float2 uv, float4 reflection_info)
{
    float4 effect_info = g_texture_color.Sample(g_sampler_buffer, uv); // uv, mask, visibility
    float2 screen_uv = reflection_info.rg;
    float visibility = reflection_info.a;

    float3 color_none = 0;
    float3 color = g_texture_color.Sample(g_sampler_buffer, screen_uv).rgb;
    float4 out_color = float4(color, visibility); // float4(lerp(color_none, color, visibility), visibility);
    
    return out_color;
}

// end interface for sampling textuers ====

// ssr utils ====

struct PixelOut
{
    // rg: uv
    // b: mask
    // a: visibility
    float4 reflection_info : SV_Target0;
    float4 reflection_color : SV_Target1;
};

PixelOut main(VertexOut input) : SV_Target
{
    // get screen size
    uint width, height;
    GetScreenSize(width, height);
    float2 texSize = float2(width, height);
    // get texture coordinate
    float2 texCoord = input.uv;

    PixelOut pixel_out;
    pixel_out.reflection_info = float4(0, 0, 0, 0);
    pixel_out.reflection_color = float4(0, 0, 0, 0);

    // get start positon info
    SurfaceData surface_v = GetSurfaceDataAt(texCoord);
    float4 position_from = surface_v.position;
    float reflection_mask = GetReflectionMask(surface_v);
    pixel_out.reflection_info.b = reflection_mask;
    
     // early out conditions
    if (position_from.w <= 0.0f || reflection_mask <= 0.0f)
    {
        // TODO: specular
        return pixel_out;
    }

    // view space reflection 
    float3 unitPositionFrom = normalize(position_from.xyz);
    float3 normal = surface_v.normal;
    float3 pivot = normalize(reflect(unitPositionFrom, normal));

    float4 startView = float4(position_from.xyz, 1.0f);
    float4 endView = float4(position_from.xyz + pivot * g_max_distance, 1.0f);

    // Project start and end points to screen space
    float4 startFrag = mul(startView, g_matrix_proj);
    startFrag.xyz /= startFrag.w;
    startFrag.xy = startFrag.xy * float2(0.5f, -0.5f) + 0.5f;
    startFrag.xy *= texSize;


    float4 endFrag = mul(endView, g_matrix_proj);
    endFrag.xyz /= endFrag.w;
    endFrag.xy = endFrag.xy * float2(0.5f, -0.5f) + 0.5f;
    endFrag.xy *= texSize;

    float2 frag = startFrag.xy;
    float2 uv = frag / texSize;

    // Screen-space ray setup
    float deltaX = endFrag.x - startFrag.x;
    float deltaY = endFrag.y - startFrag.y;
    float useX = abs(deltaX) >= abs(deltaY) ? 1.0f : 0.0f;
    float delta = lerp(abs(deltaY), abs(deltaX), useX) * saturate(g_resolution);
    delta = min(delta, 16); //
    float2 increment = float2(deltaX, deltaY) / max(delta, 0.01f);

    float search0 = 0.0f;
    float search1 = 0.0f;
    int hit0 = 0;
    int hit1 = 0;

    float viewDistance;
    float depth;

    // float4 positionTo = positionFrom;
    float invStartZ = 1.0f / startView.z;
    float invEndZ = 1.0f / endView.z;
    // Coarse ray marching
    [loop]
    for (int i = 0; i < (int)delta; ++i)
    {
        frag += increment;
        uv = frag / texSize;
        float depth_at_uv = GetDepthAt(uv);

        search1 = lerp(
            (frag.y - startFrag.y) / deltaY,
            (frag.x - startFrag.x) / deltaX,
            useX);

        search1 = saturate(search1);

        // viewDistance =
        //     (startView.z * endView.z) /
        //     lerp(endView.z, startView.z, search1);
        float currentInvRayDepth = lerp(invStartZ, invEndZ, search1);
        viewDistance = 1.0f / currentInvRayDepth;

        depth = viewDistance - depth_at_uv;

        if (depth > 0 && depth < g_thickness)
        {
            hit0 = 1;
            break;
        }
        else
        {
            search0 = search1;
        }
    }

    // Binary search refinement
    search1 = search0 + (search1 - search0) * 0.5f;
    int steps = g_steps;
    steps *= hit0;

    [loop]
    for (int i = 0; i < steps; ++i)
    {
        frag = lerp(startFrag.xy, endFrag.xy, search1);
        uv = frag / texSize;
        float depth_at_uv = GetDepthAt(uv);

        // viewDistance =
        //     (startView.z * endView.z) /
        //     lerp(endView.z, startView.z, search1);
        float currentInvRayDepth = lerp(invStartZ, invEndZ, search1);
        viewDistance = 1.0f / currentInvRayDepth;

        depth = viewDistance - depth_at_uv;

        if (depth > 0 && depth < g_thickness)
        {
            hit1 = 1;
            search1 = search0 + (search1 - search0) * 0.5f;
        }
        else
        {
            float temp = search1;
            search1 = search1 + (search1 - search0) * 0.5f;
            search0 = temp;
        }
    }

    
    SurfaceData surface_to_v = GetSurfaceDataAt(uv);
    // visibility weighting
    float visibility =
        hit1 *
        surface_to_v.position.w *
        (1.0f - max(dot(-unitPositionFrom, pivot), 0.0f)) *
        (1.0f - saturate(depth / g_thickness)) *
        (1.0f - saturate(length(surface_to_v.position.xyz - position_from.xyz) / g_max_distance)) *
        (uv.x >= 0 && uv.x <= 1 ? 1 : 0) *
        (uv.y >= 0 && uv.y <= 1 ? 1 : 0);

    visibility = saturate(visibility);

    pixel_out.reflection_info.rg = uv;
    pixel_out.reflection_info.a = visibility;
    pixel_out.reflection_color = GetReflectionColor(input.uv, pixel_out.reflection_info);

    return pixel_out;
}
