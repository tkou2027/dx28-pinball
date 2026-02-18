#ifndef __SHADING__
#define __SHADING__
// shading common
// world space
struct SurfaceData
{
    float4 albedo;
    float4 position; // w = 0 for nothing
    float3 normal;
    // material
    float metallic;
    float specular;
    float roughness;
    uint shading_model;
};

// lights ====
struct LightDirectional
{
    float3 direction_w;
    float _padding_direction_w;
    float3 color;
    float _padding_color;
};

struct LightPoint
{
    float3 position_w;
    float _padding_position_w;
    float3 color;
    float _padding_color;
};

// scene lighting
struct LightScene
{
    LightDirectional dir_light;
    LightPoint point_light[48];
    int num_point_lights;
    float _padding_light_num_1;
    float _padding_light_num_2;
    float _padding_light_num_3;
};
// lights end ====


// shading utils ====
float ComputeLambert(float3 normal, float3 light_dir)
{
    // n_dot_l
    return max(dot(normal, light_dir), 0.0f);
}

float ComputeHalfLambert(float3 normal, float3 light_dir)
{
    return max(dot(normal, light_dir) * 0.5f + 0.5f, 0.0f);
}

// blinn-phong specular
float ComputeSpecular(float3 normal, float3 light_dir, float3 view_dir, float shiness)
{
    float3 half_dir = normalize(view_dir + light_dir);
    float n_dot_h = max(dot(normal, half_dir), 0.0f);
    return pow(n_dot_h, shiness);
}

// rim / fresnel
float ComputeRim(float3 normal, float3 view_dir, float power)
{
    float v_dot_n = dot(view_dir, normal);
    float rim_intencity = max(0.0, 1.0 - v_dot_n);
    rim_intencity = pow(rim_intencity, power);
    return rim_intencity;
}

// shading utils end ====

// blinn-phong ====
float3 ComputeShadingBlinnPhongDirectional(SurfaceData surface_w, LightDirectional light, float3 view_dir)
{
    float diffuse = ComputeLambert(surface_w.normal, -light.direction_w);
    float specular = ComputeSpecular(surface_w.normal, -light.direction_w, view_dir, 32.0f); // TODO shiness
    float3 lighting = 0.1f + light.color * diffuse + light.color * specular; // same color for diffuse and specular, no ambient
    return lighting * surface_w.albedo.rgb;
}

float3 ComputeShadingBlinnPhongPoint(SurfaceData surface_w, LightPoint light, float3 view_dir)
{
    float3 to_light = light.position_w - surface_w.position.xyz;

    float3 light_dir = normalize(to_light);
    float light_dist = length(to_light);
    float dist_sqrt = clamp(light_dist * light_dist, 1.0f, 100.0f) * 0.1f;
    float diffuse = ComputeLambert(surface_w.normal, light_dir);
    float specular = ComputeSpecular(surface_w.normal, light_dir, view_dir, 32.0f); // TODO shiness
    float3 lighting = (1.0f / dist_sqrt) * light.color * diffuse + light.color * specular; // same color for diffuse and specular, no ambient
    return lighting * surface_w.albedo.rgb;
}

float3 ComputeShadingBlinnPhong(SurfaceData surface_w, LightScene light_scene, float3 view_pos)
{
    // view dir
    float3 view_dir = normalize(view_pos - surface_w.position.xyz);
    // directional light
    float3 color = ComputeShadingBlinnPhongDirectional(surface_w, light_scene.dir_light, view_dir);
    // point lights
    //[unroll]
    //for (int i = 0; i < light_scene.num_point_lights; i++)
    //{
    //    color += ComputeShadingBlinnPhongPoint(surface_w, light_scene.point_light[i], view_dir);
    //}
    // TODO: multiple lights
    return color;
}
// blinn-phong end


// toon
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

float3 ComputeLightingRamp(
    Texture2D texture_ramp, SamplerState sampler_ramp,
    float3 normal, float3 viewDir, float3 lightDir, float4 albedo, float ramp_offset, float shadow_lit)
{
    float halfLambert = ComputeHalfLambert(normal, lightDir);
    halfLambert = smoothstep(0.0, 0.5, halfLambert);
    halfLambert = clamp(halfLambert, 0.01f, 0.99f);
    // halfLambert = min(halfLambert, shadow_lit);
    float3 rampColor = texture_ramp.Sample(sampler_ramp, float2(halfLambert, ramp_offset)).rgb;
    
    float3 rimColor = ComputeRimLight(normal, lightDir, viewDir, albedo.rgb);
    float emmisionColor = 0.0f; //ComputeEmission(albedo);
    
    float3 color = rampColor * albedo.rgb + rimColor * 0.1f + emmisionColor;
    return color;
}

float3 ComputeShadingToon(Texture2D texture_ramp, SamplerState sampler_ramp,
    float3 position_w, float3 normal_w, float4 albedo, float ramp_offset,
    float3 light_dir_w, float3 view_pos_w)
{
    // view dir
    float3 view_dir = normalize(view_pos_w - position_w);
    return ComputeLightingRamp(texture_ramp, sampler_ramp,
        normal_w, view_dir, light_dir_w, albedo, ramp_offset, 1.0f);
}

// toon

// pbr


#endif