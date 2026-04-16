// reference: Efficient GPU Screen-Space Ray Tracing
#include "vertex_out/vertex_out_fullscreen.hlsli"
#include "common/buffer_camera.hlsli" 
#include "feature/g_buffer_read.hlsli"

Texture2D g_texture_color : register(t4);
Texture2D g_texture_hi_z : register(t5);

// Point sampler is critical for Hi-Z to avoid depth bleeding
SamplerState g_sampler_point_clamp : register(s1);

cbuffer BufferSSRConfig : register(b2)
{
    float g_max_distance;
    float g_stride; // float g_resolution;
    float g_thickness;
    int g_steps;
};

// Jitter Constants
// 4x4 Bayer matrix for dithering
static float g_dither[16] =
{
    0.0, 0.5, 0.125, 0.625,
    0.75, 0.25, 0.875, 0.375,
    0.187, 0.687, 0.0625, 0.562,
    0.937, 0.437, 0.812, 0.312
};

// Reconstruct surface data from G-Buffer
SurfaceData GetSurfaceDataAt(float2 uv)
{
    return GetSurfaceDataViewFromGBuffer(uv, g_matrix_proj, g_matrix_proj_inverse, g_matrix_view);
}

// Reconstruct linear view-space Z (Standard Z: 0 is near, 1 is far)
float GetDepthLinear(float2 uv, float mip)
{
    float raw_depth = g_texture_hi_z.SampleLevel(g_sampler_point_clamp, uv, mip).r;
    // Standard Z linear reconstruction: 1.0 / (z * m23 + m33)
    return 1.0f / (raw_depth * g_matrix_proj_inverse[2][3] + g_matrix_proj_inverse[3][3]);
}

float ComputeStepScale(float mip)
{
    float step_scale = max(g_stride, exp2(mip));
    return step_scale;
}

float2 ComputeMipTextureSize(int mip)
{
    uint mip_w, mip_h, out_mip_levels;
    g_texture_hi_z.GetDimensions(mip, mip_w, mip_h, out_mip_levels);
    return float2(mip_w, mip_h);
}

struct PixelOut
{
    float4 reflection_info : SV_Target0;
    float4 reflection_color : SV_Target1;
};

PixelOut main(VertexOut input) : SV_Target
{
    uint width, height;
    g_texture_buffer_a.GetDimensions(width, height);
    float2 screen_size = float2(width, height);
    PixelOut pixel_out = (PixelOut) 0;

    // 1. early out by material =======
    SurfaceData surface_v = GetSurfaceDataAt(input.uv);
    float reflection_mask = step(0.9, surface_v.specular) * (surface_v.shading_model == 0);
    if (surface_v.position.w <= 0.0f || reflection_mask <= 0.0f)
    {
        return pixel_out;
    }

    // 2. viw space start / end =======
    float3 v_dir = normalize(surface_v.position.xyz);
    float3 r_dir = normalize(reflect(v_dir, surface_v.normal));
    float3 start_view = surface_v.position.xyz;
    float3 end_view = start_view + r_dir * g_max_distance;

    // 3. start / end values =======
    // h: homogeneous space position
    float4 start_h = mul(float4(start_view, 1.0), g_matrix_proj);
    float4 end_h = mul(float4(end_view, 1.0), g_matrix_proj);
    // k: inverse view space depth
    float start_k = 1.0 / start_h.w;
    float end_k = 1.0 / end_h.w;
    // s: screen space position(pixel coordinates)
    float2 start_s = (start_h.xy * start_k * float2(0.5, -0.5) + 0.5) * screen_size;
    float2 end_s = (end_h.xy * end_k * float2(0.5, -0.5) + 0.5) * screen_size;
    // q: view space (after homogeneous division)
    float3 start_q = start_view * start_k;
    float3 end_q = end_view * end_k;

    // 4. DDA =======
    float2 delta_s = end_s - start_s;
    bool permute = false;
    if (abs(delta_s.x) < abs(delta_s.y))
    {
        permute = true;
        delta_s = delta_s.yx;
        start_s = start_s.yx;
        end_s = end_s.yx;
    }

    float step_dir = sign(delta_s.x);
    float inv_dx = step_dir / delta_s.x;
    float2 dp = float2(step_dir, inv_dx * delta_s.y);
    float3 dq = (end_q - start_q) * inv_dx;
    float dk = (end_k - start_k) * inv_dx;
    // stride scale
    //dp *= g_stride;
    //dq *= g_stride;
    //dk *= g_stride;
    
    // jitter

    // 5. Ray Marching ======
    float pre_z = start_view.z;
    // marched values
    float mip_level = 0.0;
    float2 p = start_s; // P: screen space position
    float3 q = start_q; // Q: view space position
    float k = start_k;  // K: inverse view space depth
    
    // jitter
    int dither_idx = (start_s.x % 4) * 4 + (start_s.y % 4);
    float jitter = g_dither[dither_idx];
    p += dp * jitter;
    q += dq * jitter;
    k += dk * jitter;

    bool hit = false;
    float2 hit_uv = 0;
    float2 screen_size_inv = 1.0 / screen_size;
    float step_scale = ComputeStepScale(mip_level);
    // float2 mip_texutre_size = ComputeMipTextureSize(mip_level);
    [loop]
    for (int i = 0; i < g_steps; i++)
    {
        float2 next_p = p + dp * step_scale;
        float3 next_q = q + dq * step_scale;
        float next_k = k + dk * step_scale;

        // current segment z range
        float ray_z_next = next_q.z / next_k;
        //(dq.z * step_scale * 0.5 + next_q.z) / (dk * step_scale * 0.5 + next_k);

        // Sorting Z for comparison
        float temp_min = min(pre_z, ray_z_next);
        float temp_max = max(pre_z, ray_z_next);

        hit_uv = (permute ? next_p.yx : next_p) * screen_size_inv;

        // Screen boundary check
        if (any(hit_uv < 0.0) || any(hit_uv > 1.0))
        {
            break;
        }

        float surface_z = GetDepthLinear(hit_uv, mip_level);
        // use a small bias to avoid false positives
        bool behind = temp_max > (surface_z + 0.05);

        if (!behind)
        {
            // Advance ray and try to increase mip level for faster jumping
            p = next_p;
            q = next_q;
            k = next_k;
            pre_z = ray_z_next;
            mip_level = min(mip_level + 1.0, 3.0);
            step_scale = ComputeStepScale(mip_level);
            // mip_texutre_size = ComputeMipTextureSize(mip_level);
        }
        else
        {
            if (mip_level <= 0.0)
            {
                // Detailed check at Mip 0 with thickness tolerance
                if (abs(temp_min - surface_z) <= g_thickness)
                {
                    hit = true;
                    break;
                }
                // Penetrated a thin object: skip and continue
                p = next_p;
                q = next_q;
                k = next_k;
                pre_z = ray_z_next;
            }
            else
            {
                // backtrack and refine at a lower mip level
                mip_level = max(mip_level - 1.0, 0.0);
                step_scale = ComputeStepScale(mip_level);
                // mip_texutre_size = ComputeMipTextureSize(mip_level);
            }
        }
    }

    // shading
    if (hit)
    {
        SurfaceData hit_data = GetSurfaceDataAt(hit_uv);
        float dist_fade = 1.0 - saturate(length(hit_data.position.xyz - start_view) / g_max_distance);
        float edge_fade = saturate(8.0 * min(min(hit_uv.x, 1.0 - hit_uv.x), min(hit_uv.y, 1.0 - hit_uv.y)));
        float v_dot_r = 1.0 - max(dot(-v_dir, r_dir), 0.0);

        float visibility = dist_fade * edge_fade * v_dot_r;
        
        pixel_out.reflection_info.rg = hit_uv;
        pixel_out.reflection_info.a = visibility;
        pixel_out.reflection_color = float4(g_texture_color.SampleLevel(g_sampler_buffer, hit_uv, 0).rgb, visibility);
    }

    return pixel_out;
}