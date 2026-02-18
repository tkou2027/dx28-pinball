#include "vertex_out/vertex_out_fullscreen.hlsli"
#include "feature/tone_mapping.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 main(VertexOut input) : SV_Target
{
    // tone mapping
    float3 hdr = g_texture.Sample(g_sampler, input.uv).rgb;
    float3 ldr = ToneMapACES(hdr);
    
    // no more manual gamma correction here
    // as the swapchain is created as SRGB
    // float3 gamma_corrected = pow(ldr, 1.0f / 2.2f);

    return float4(ldr, 1.0f);
}