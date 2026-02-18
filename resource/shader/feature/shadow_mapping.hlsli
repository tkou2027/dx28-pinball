#ifndef __SHADOW_MAPPING__
#define __SHADOW_MAPPING__

struct LightDirectionalShadow
{
    float4x4 matrix_light_view_proj;
};

struct LightShadowScene
{
    LightDirectionalShadow dir_light_shadow;
};

// basic
float ComputeShadow(
    Texture2D texture_shadow,
    SamplerComparisonState sampler_shadow,
    float4 pos_view_light, float3 normal, float3 lightDir)
{
    float3 dir_light_pos = pos_view_light.xyz / pos_view_light.w;
    dir_light_pos.x = dir_light_pos.x * 0.5f + 0.5f;
    dir_light_pos.y = dir_light_pos.y * -0.5f + 0.5f;

    float percentLit = 0.0f;
    // float2 texelSize = 1.0f / 1024.0f;
    float bias = 0.001; // max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        int2 offset = int2(i % 3 - 1, i / 3 - 1);
        percentLit += texture_shadow.SampleCmpLevelZero(sampler_shadow, dir_light_pos.xy, dir_light_pos.z - bias, offset).x;
    }
    percentLit /= 9.0f;
    percentLit = saturate(percentLit);
    return percentLit;
}

#endif