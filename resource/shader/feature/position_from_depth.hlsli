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
#endif