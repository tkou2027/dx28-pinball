#include "vertex_out/vertex_out_base.hlsli"
// gbuffer
struct PixelOut
{
    float4 buffer_a : SV_Target0; // world normal
    float4 buffer_b : SV_Target1; // metallic, specular, roughness, shading model
    float4 buffer_c : SV_Target2; // base color
    float4 buffer_d : SV_Target3; // output hdr color, emmision
};

// textures
Texture2D g_texture_diffuse : register(t0);
Texture2D g_texture_emission  : register(t1);
// pbr
Texture2D g_texture_normal : register(t2);
Texture2D g_texture_metallic : register(t3);
Texture2D g_texture_roughness : register(t4);

// TextureCube g_texture_environment : register(t1);
SamplerState g_sampler_texture : register(s0);

// materails
cbuffer MaterialDefault : register(b0)
{
    float4 g_base_color;

    float g_metallic;
    float g_specular;
    float g_roughness;
    uint g_shading_model;

    float3 g_emission_color;
    float g_emission_intensity;
};

// hack
float3 GetNormalFromMap(float3 worldPos, float2 texCoords, float3 worldNormal)
{
    float3 tangentNormal = g_texture_normal.Sample(g_sampler_texture, texCoords).xyz * 2.0 - 1.0;

    float3 Q1 = ddx(worldPos);
    float3 Q2 = ddy(worldPos);
    float2 st1 = ddx(texCoords);
    float2 st2 = ddy(texCoords);

    float3 N = normalize(worldNormal);
 
    float3 T = normalize(Q1 * st2.y - Q2 * st1.y);
    float3 B = -normalize(cross(N, T));

    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(tangentNormal, TBN));
}


PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    // base color
    float4 albedo = g_base_color * g_texture_diffuse.Sample(g_sampler_texture, pixel_in.uv);
    clip(albedo.a - 0.1);
    pixel_out.buffer_c = albedo;
    
    // normal
    float3 normal_w = normalize(pixel_in.normal_w);
    float3 tangent_w = normalize(pixel_in.tangent_w.xyz);
    float3 bitangent_w = cross(normal_w, tangent_w.xyz) * sign(pixel_in.tangent_w.w);
    float3x3 tbn = float3x3(tangent_w, bitangent_w, normal_w);
    float3 normal_map = g_texture_normal.Sample(g_sampler_texture, pixel_in.uv).xyz * 2.0f - 1.0f;
    float3 normal = normalize(mul(normal_map, tbn));
    
    // float3 normal = GetNormalFromMap(pixel_in.position_w.xyz, pixel_in.uv, pixel_in.normal_w);
    pixel_out.buffer_a.xyz = normal; //normalize(pixel_in.normal_w);
    pixel_out.buffer_a.w = 0.0f;
    // material
    pixel_out.buffer_b.r = g_metallic * g_texture_metallic.Sample(g_sampler_texture, pixel_in.uv);
    pixel_out.buffer_b.g = g_specular;
    pixel_out.buffer_b.b = g_roughness * g_texture_roughness.Sample(g_sampler_texture, pixel_in.uv);
    pixel_out.buffer_b.a = (float) g_shading_model / 16.0f; //1.0f;//asfloat(g_shading_model);

    // TODO: calc emission
    float4 emission = g_texture_emission.Sample(g_sampler_texture, pixel_in.uv);
    emission.rgb = emission.rgb * g_emission_color * g_emission_intensity; // TODO: calc emission
    pixel_out.buffer_d = emission;

    return pixel_out;
}