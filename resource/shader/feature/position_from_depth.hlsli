#ifndef __POSITION_FROM_DEPTH__
#define __POSITION_FROM_DEPTH__
// matrix_inverse:
//   proj_inverse for view space
//   view_proj_inverse for world space
float4 GetPositionFromDepth(float2 uv, float depth, float4x4 matrix_inverse)
{
    if (depth == 1.0f)
    {
        float4 none = 0.0f;
        return none;
    }
    // uv -> clip
    float ndc_x = uv.x * 2.0f - 1.0f; // [0, 1] -> [-1, 1]
    float ndc_y = 1.0f - uv.y * 2.0f; // [0, 1] -> [1, -1]
    float4 clip = float4(ndc_x, ndc_y, depth, 1.0f);

    // clip -> world(view)
    float4 world = mul(clip, matrix_inverse);
    world.xyz /= world.w;
    world.w = 1.0f;
 
    return world;
}


float4 GetPositionFromDepth(float2 uv, float depth, float4x4 proj, float4x4 proj_inverse)
{
    if (depth == 1.0f)
    {
        float4 none = 0.0f;
        return none;
    }
    // uv -> clip
    float ndc_x = uv.x * 2.0f - 1.0f; // [0, 1] -> [-1, 1]
    float ndc_y = 1.0f - uv.y * 2.0f; // [0, 1] -> [1, -1]
    
    // view(world) depth
    float view_z = 1.0 / (depth * proj_inverse[2][3] + proj_inverse[3][3]);
    
    float4 clip = float4(ndc_x, ndc_y, depth, 1.0f);

    float3 ray;
    ray.x = ndc_x / proj[0][0];
    ray.y = ndc_y / proj[1][1];
    ray.z = 1.0;

    float4 world;
    world.xyz = ray * view_z;
    world.w = 1.0;
 
    return world;
}
#endif