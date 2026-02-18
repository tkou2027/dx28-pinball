#include "common.hlsli"
#include "common_cel.hlsli"

struct PixelOut
{
    float4 color : SV_Target;
    float4 color_test : SV_Target1;
};

Texture2D diffuseTexture : register(t0);
Texture2D rampTexture : register(t1);
Texture2D dirShadowTexture : register(t2);
SamplerState texSampler : register(s0);
SamplerComparisonState shadowSampler : register(s1);

// --------------------------------------------
// Light and material parameters
// --------------------------------------------

// Fixed values for testing blinn-phong
static const float3 LIGHT_COLOR = float3(1.0f, 1.0f, 1.0f);
// Material constants
static const float3 AMBIENT_COLOR = float3(0.2f, 0.2f, 0.2f);
static const float3 SPECULAR_COLOR = float3(1.0f, 1.0f, 1.0f);
static const float SHININESS = 64.0f;

// --------------------------------------------
// Helper functions
// --------------------------------------------
// Compute diffuse term
float3 ComputeLambert(float3 normal, float3 lightDir, float3 lightColor)
{
    float NdotL = max(dot(normal, -lightDir), 0.0f);
    return lightColor * NdotL;
}

float ComputeHalfLambert(float3 normal, float3 lightDir)
{
    return max(dot(normal, -lightDir) * 0.5f + 0.5f, 0.0f);
}

// Compute specular term (Blinn-Phong)
float3 ComputeSpecular(float3 normal, float3 lightDir, float3 viewDir, float3 specularColor)
{
    float3 halfDir = normalize(viewDir - lightDir);
    float NdotH = max(dot(normal, halfDir), 0.0f);
    float spec = pow(NdotH, SHININESS);
    return specularColor * spec;
}

float3 ComputeRimLight(float3 normal, float3 lightDir, float3 viewDir, float3 rimColor)
{
    float intensity = 1.0f;
    float radius = 0.2f;
    float NdotL = max(dot(normal, -lightDir), 0.0f);
    float NdotV = max(dot(normal, viewDir), 0.0f);
    return (1 - smoothstep(radius, radius + 0.03, NdotV)) * intensity * (1 - (NdotL * 0.5 + 0.5)) * rimColor;
}

float3 ComputeEmission(float4 baseColor)
{
    float intensity = 0.1f;
    return baseColor.a * baseColor * intensity;
}

// Combine lighting
float3 ComputeLightingBlinnPhong(float3 normal, float3 viewDir, float3 lightDir, float3 albedo)
{
    float3 diffuse = ComputeLambert(normal, lightDir, LIGHT_COLOR);
    float3 specular = ComputeSpecular(normal, lightDir, viewDir, SPECULAR_COLOR);
    
    float3 color = AMBIENT_COLOR * albedo + (diffuse + specular) * albedo;
    return color;
}

float ComputeShadow(float4 pos_view_light, float3 normal, float3 lightDir)
{
    float3 dir_light_pos = pos_view_light.xyz / pos_view_light.w;
    dir_light_pos.x = dir_light_pos.x * 0.5f + 0.5f;
    dir_light_pos.y = dir_light_pos.y * -0.5f + 0.5f;

    float percentLit = 0.0f;
    // float2 texelSize = 1.0f / 1024.0f;
    float bias = 0.001; //max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        int2 offset = int2(i % 3 - 1, i / 3 - 1);
        // float dir_shadow_depth = dirShadowTexture.Sample(texSampler, dir_light_pos.xy + offset * texelSize).x;
        // percentLit += (dir_light_pos.z - bias) < dir_shadow_depth;
        percentLit += dirShadowTexture.SampleCmpLevelZero(shadowSampler, dir_light_pos.xy, dir_light_pos.z - bias, offset).x;
    }
    percentLit /= 9.0f;
    percentLit = clamp(percentLit, 0.01f, 0.99f);
    // percentLit = smoothstep(0.0, 0.5, percentLit);
   
    // percentLit = saturate(percentLit);
    float NdotL = dot(normal, lightDir);
    percentLit = lerp(percentLit, 0.99f, smoothstep(0.4f, 0.7f, NdotL));
    
    percentLit = clamp(percentLit, 0.01f, 0.99f);
    
    return percentLit;
}

float3 ComputeLightingRamp(float3 normal, float3 viewDir, float3 lightDir, float4 albedo, float ramp_offset, float shadow_lit)
{
    float halfLambert = ComputeHalfLambert(normal, lightDir);
    halfLambert = smoothstep(0.0, 0.5, halfLambert);
    halfLambert = clamp(halfLambert, 0.01f, 0.99f);
    halfLambert = min(halfLambert, shadow_lit);
    float3 rampColor = rampTexture.Sample(texSampler, float2(halfLambert, ramp_offset)).rgb;
    
    float3 rimColor = ComputeRimLight(normal, lightDir, viewDir, albedo.rgb);
    float emmisionColor = ComputeEmission(albedo);
    
    float3 color = rampColor * albedo.rgb + rimColor + emmisionColor;
    return color;
}

PixelOut main(VertexOutCel pixelIn)
{
    PixelOut pixelOut;
    
    // const float4 diffuleColor = diffuseTexture.Sample(texSampler, pixelIn.uv);
    // // TODO gamma
    // pixelOut.color = diffuleColor; // float4(pixelIn.normal, 1.0f); // diffuleColor; // float4(pixelIn.uv, 0.0, 1.0);
    // // pixelOut.color.w = 1.0f;
    
    float4 albedo = diffuseTexture.Sample(texSampler, pixelIn.uv);
    
    float3 normal = normalize(pixelIn.normal_world);
    
    float3 viewDir = normalize(camera_position - pixelIn.position_world.xyz);
    
    float3 lightDir = normalize(directional_light.direction);
    
    float3 lighting_phong = ComputeLightingBlinnPhong(normal, viewDir, lightDir, albedo);
    
    float shadow_lit = 1.0; // ComputeShadow(pixelIn.position_dir_light, normal, lightDir);
    float3 lighting_cel = ComputeLightingRamp(normal, viewDir, lightDir, albedo, material_cel.ramp_offset, shadow_lit);
    
    float w0 = step(material_cel.shader_mode, 0.25);
    float w1 = step(0.25, material_cel.shader_mode) * step(material_cel.shader_mode, 0.75);
    float w2 = step(0.75, material_cel.shader_mode);
    
    float3 lighting = w0 * albedo.rgb + w1 * lighting_phong + w2 * lighting_cel;
    lighting = lighting + float3(1.0, 0.6, 1.0) * material_cel.use_alter_texture;
    // if (material_cel.use_alter_texture)
    // {
    //     lighting = (1.0f - lighting) + float3(0.4f, 0.0, 0.0);
    // 
    // }
    pixelOut.color = float4(lighting, 1.0f);
    
    pixelOut.color_test = float4(0.0, 0.0, 0.0, 1.0);
    //float brightness = dot(lighting.rgb, float3(0.2126, 0.7152, 0.0722));
    //if (brightness > 0.9)
    //{
    //    pixelOut.color_test = pixelOut.color;
    //}
    //else
    //{
    //    pixelOut.color_test = float4(0.0, 0.0, 0.0, 1.0);
    //
    //}
    ////pixelOut.color = float4(normal, 1.0f);
    
    // // shadow
    // float3 dir_light_pos = pixelIn.position_dir_light.xyz / pixelIn.position_dir_light.w;
    // dir_light_pos.x = dir_light_pos.x * 0.5f + 0.5f;
    // dir_light_pos.y = dir_light_pos.y * -0.5f + 0.5f;
    // // dir_light_pos.x = -dir_light_pos.x;
    // // dir_light_pos.y = -dir_light_pos.y;
    // // float dir_shadow_depth = dirShadowTexture.Sample(texSampler, dir_light_pos.xy).x;
    // // float shadow = dirShadowTexture.SampleCmpLevelZero(shadowSampler, dir_light_pos.xy, dir_light_pos.z);
    // // float percentLit = 0.0f;
    // // percentLit = (dir_light_pos.z - 0.01f) < dir_shadow_depth;
    // // percentLit = clamp(percentLit + 0.8f, 0.0f, 1.0f);
    // 
    // 
    // float percentLit = 0.0f;
    // float2 texelSize = 1.0f / 1024.0f;
    // float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
    // [unroll]
    // for (int i = 0; i < 9; ++i)
    // {
    //     int2 offset = int2(i % 3 - 1, i / 3 - 1);
    //     // float dir_shadow_depth = dirShadowTexture.Sample(texSampler, dir_light_pos.xy + offset * texelSize).x;
    //     // percentLit += (dir_light_pos.z - bias) < dir_shadow_depth;
    // 
    //     percentLit += dirShadowTexture.SampleCmpLevelZero(shadowSampler, dir_light_pos.xy, dir_light_pos.z - bias, offset).x;
    // }
    // percentLit /= 9.0f;
    // 
    // percentLit = clamp(percentLit + 0.5f, 0.0f, 1.0f);
    // 
    // float NdotL = dot(normal, lightDir);
    // percentLit = lerp(percentLit, 1.0f, smoothstep(0.2f, 0.5f, NdotL));
    // 
    // pixelOut.color.rgb *= percentLit;
    
    // pixelOut.color.r = dir_shadow_depth;
    // pixelOut.color.yz = 0.0f;
    
    //pixelOut.color.rg = dir_light_pos.xy;
    //pixelOut.color.x = 0.0f;
    //pixelOut.color.z = 0.0f;
    
    // pixelOut.color.w = 1.0f;

    // pixelOut.color.rg = dir_shadow_depth;
    // pixelOut.color.b = 0.0f;
    // pixelOut.color.w = 1.0f;
    
    // float percentLit = ComputeShadow(pixelIn.position_dir_light, normal, lightDir);
    // pixelOut.color.rgb *= percentLit;
    
    return pixelOut;
}