#ifndef __SKELETAL_ANIMATION__
#define __SKELETAL_ANIMATION__

float4 GetPositionSkinned(float4 position_l,
    float4 bone_indeces, float4 bone_weights, StructuredBuffer<float4x4> bone_matrices)
{
    float4 skinned = float4(0, 0, 0, 0);
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        int bone_index = bone_indeces[i];
        float bone_weight = bone_weights[i];
        if (bone_index >= 0)
        {
            skinned += mul(position_l, bone_matrices[bone_index]) * bone_weight;
        }
        if (bone_index < 0 && i == 0)
        {
            skinned = position_l;
            // no break for unrolling
        }
    }
    return skinned;
}

float3 GetNormalSkinned(float3 normal_l,
    float4 bone_indices, float4 bone_weights, StructuredBuffer<float4x4> bone_matrices)
{
    float3 skinned = float3(0, 0, 0);

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        int bone_index = bone_indices[i];
        float bone_weight = bone_weights[i];

        if (bone_index >= 0)
        {
            skinned += mul(normal_l, (float3x3) bone_matrices[bone_index]) * bone_weight;
        }
        if (bone_index < 0 && i == 0)
        {
            skinned = normal_l;
        }
    }
    return normalize(skinned);
}

#endif