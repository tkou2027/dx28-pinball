#ifndef __TEXTURE_PARTICLE__
#define __TEXTURE_PARTICLE__
struct TextureParticle
{
    float3 position;
    float3 velocity;
    //float3 normal;
    float2 uv;
    float life;
    float delay;
};
#endif