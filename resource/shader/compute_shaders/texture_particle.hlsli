#ifndef __TEXTURE_PARTICLE__
#define __TEXTURE_PARTICLE__
struct TextureParticle
{
    float3 position;
    float3 velocity;

    float2 uv;

    float3 right;
    float3 up;

    float life;
    float delay;
};
#endif