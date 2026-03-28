 #include "vertex_out/vertex_out_fullscreen.hlsli"
 
Texture2D g_texture_base : register(t0);
Texture2D g_texture_reflection : register(t1);
Texture2D g_texture_g_buffer_material : register(t2); // g buffer material, roughness in z
 
SamplerState g_sampler : register(s0);
 
float4 main(VertexOut input) : SV_Target
{
    float3 base_color = g_texture_base.Sample(g_sampler, input.uv).rgb;
 
    float roughness = g_texture_g_buffer_material.Sample(g_sampler, input.uv).z;
    float target_level = roughness * roughness * 9.0f + 1.0f; // max mip level is 5
     
    float4 reflection = g_texture_reflection.SampleLevel(g_sampler, input.uv, target_level);
    float visibility = reflection.a;
     // visibility = pow(visibility, 0.5f);
    visibility *= saturate(1.0f - roughness * roughness);

    float3 color = lerp(base_color, reflection.rgb, visibility * 0.3);
 
    return float4(color, 1.0f);
}
