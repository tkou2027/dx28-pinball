#include "common_skybox.hlsli"

TextureCube cubeTexture : register(t1);
SamplerState texSampler : register(s0);

float4 main(VertexOutSkybox pixelIn) : SV_TARGET
{
    float4 color = cubeTexture.Sample(texSampler, pixelIn.posL);
    return color;
}