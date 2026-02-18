// reference: learnopengl
#ifndef __SHADING_PBR__
#define __SHADING_PBR__
#include "../common/math_util.hlsli"
#include "shading.hlsli"

// D
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// G
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max((1.0 - roughness).xxx, F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


float3 ComputeBRDFCookTorrance(
    float3 N, float3 V, float3 L,
    float metallic, float roughness, float3 albedo
)
{
    float3 F0 = lerp(0.04f.xxx, albedo, metallic);

    float3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
           
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    float3 specular = numerator / denominator;
        
    // kS is equal to fresnel
    float3 k_s = F;
    // energy conservation
    float3 k_d = 1.0 - k_s;
    // pure metals have no diffuse light
    k_d *= 1.0 - metallic;

    // scale light by NdotL
    // float NdotL = max(dot(N, L), 0.0);

    // outgoing radiance
    return (k_d * albedo / PI + specular);// * NdotL;
}

float3 ComputeShadingCookTorrancePointLight(
    SurfaceData surface_w, LightPoint light, float3 view_dir)
{
    float3 to_light = light.position_w - surface_w.position.xyz;
    float distance_sq = dot(to_light, to_light);
    float attenuation = 1.0 / (distance_sq + 0.0001);
    float3 radiance = light.color * attenuation;
    
    float3 light_dir = normalize(to_light);
    float3 brdf = ComputeBRDFCookTorrance(
        surface_w.normal, view_dir, light_dir,
        surface_w.metallic, surface_w.roughness, surface_w.albedo.rgb
    );
    float n_dot_l = max(dot(surface_w.normal, light_dir), 0.0);

    return brdf * radiance * n_dot_l;
}

float3 ComputeShadingCookTorranceDirectionalLight(
    SurfaceData surface_w, LightDirectional light, float3 view_dir)
{
    float3 to_light = -light.direction_w;
    float3 radiance = light.color;
    float3 light_dir = normalize(to_light);

    float3 brdf = ComputeBRDFCookTorrance(
        surface_w.normal, view_dir, light_dir,
        surface_w.metallic, surface_w.roughness, surface_w.albedo.rgb
    );
    float n_dot_l = max(dot(surface_w.normal, light_dir), 0.0);

    return brdf * radiance * n_dot_l;
}

float3 ComputeShadingCookTorranceAmbient(
    SurfaceData surface_w, LightDirectional light, float3 view_dir,
    TextureCube ibl_diffuse, TextureCube ibl_specular, Texture2D brdf_lut,
    SamplerState sampler_cube, SamplerState sampler_clamp)
{
    float3 N = surface_w.normal;
    float3 V = view_dir;
    float3 R = reflect(-V, N);
    
    float3 F0 = lerp(0.04f.xxx, surface_w.albedo.rgb, surface_w.metallic);
    
    float n_dot_v = max(dot(N, V), 0.0);
    float3 F = fresnelSchlickRoughness(n_dot_v, F0, surface_w.roughness);
    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - surface_w.metallic);

    float3 irradiance = ibl_diffuse.Sample(sampler_cube, N);
    float3 diffuse = irradiance * surface_w.albedo.rgb;
    
    // TODO::
    const float MAX_REFLECTION_LOD = 8.0;
    float3 prefilteredColor = ibl_specular.SampleLevel(sampler_cube, R, surface_w.roughness * MAX_REFLECTION_LOD).rgb;
    float2 brdf = brdf_lut.Sample(sampler_clamp, float2(n_dot_v, surface_w.roughness)).rg;
    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    float ao = 0.06f; // TODO: get ao map
    float3 ambient = (kD * diffuse + specular) * ao;
    
    float3 color = ambient;

    return color;
}

//float3 ComputeShadingCookTorranceAmbientClearCoat(
//    SurfaceData surface_w, LightDirectional light, float3 view_dir,
//    TextureCube ibl_diffuse, TextureCube ibl_specular, Texture2D brdf_lut,
//    SamplerState sampler_cube, SamplerState sampler_clamp)
//{
//    float3 N = normalize(surface_w.normal);
//    float3 V = normalize(view_dir);
//    float3 R = reflect(-V, N);
//    float n_dot_v = max(dot(N, V), 0.0001f);
//    const float MAX_REFLECTION_LOD = 6.0;
//
//    // base layer
//    float3 F0 = lerp(0.04f.xxx, surface_w.albedo.rgb, surface_w.metallic);
//    float3 F = fresnelSchlickRoughness(n_dot_v, F0, surface_w.roughness);
//    
//    float3 kS = F;
//    float3 kD = (1.0 - kS) * (1.0 - surface_w.metallic);
//
//    float3 irradiance = ibl_diffuse.Sample(sampler_cube, N).rgb;
//    float3 diffuse = irradiance * surface_w.albedo.rgb;
//    
//    float3 prefilteredColor = ibl_specular.SampleLevel(sampler_cube, R, surface_w.roughness * MAX_REFLECTION_LOD).rgb;
//    float2 brdf = brdf_lut.Sample(sampler_clamp, float2(n_dot_v, surface_w.roughness)).rg;
//    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
//    
//    float3 baseLayer = (kD * diffuse + specular);
//
//    // clear coat layer
//    float clearCoat = 1.0f;
//    float ccRoughness = 0.01f;
//    float3 F0_cc = 0.04f;
//    float n_dot_v_cc = max(dot(float3(0.0f, 1.0f, 0.0f), V), 0.0f);
//    float3 R_cc = reflect(-V, float3(0.0f, 1.0f, 0.0f));
//    float3 F_cc = fresnelSchlickRoughness(n_dot_v_cc, F0_cc, ccRoughness) * clearCoat;
//    float3 prefilteredColor_cc = ibl_specular.SampleLevel(sampler_cube, R_cc, ccRoughness * MAX_REFLECTION_LOD).rgb;
//    float2 brdf_cc = brdf_lut.Sample(sampler_clamp, float2(n_dot_v_cc, ccRoughness)).rg;
//    float3 specular_cc = prefilteredColor_cc * (F_cc * brdf_cc.x + brdf_cc.y);
//
//    float ao = 1.0f;
//    float3 ambient = (baseLayer * (1.0f - F_cc) + specular_cc) * ao;
//
//    return ambient;
//}

#endif