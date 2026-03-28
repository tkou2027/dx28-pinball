Texture2D g_particle_texture : register(t0);
Texture2D g_local_texture : register(t1);
SamplerState g_sampler : register(s0);

struct PSIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 uv_local : TEXCOORD1;
    float life : TEXCOORD2;
};

struct PixelOut
{
    float4 color : SV_Target0; // color
    float4 emission : SV_Target1; // emission
};

PixelOut main(PSIn pin) : SV_TARGET
{
    PixelOut pixel_out;
    
    // pin.uv.y = 1.0f - pin.uv.y; // flip Y // TODO

    // Sample base color from particle texture using global UV
    float4 albedo = g_particle_texture.Sample(g_sampler, pin.uv);
    
    uint width, height;
    g_particle_texture.GetDimensions(width, height);
    float2 screen_size = float2(width, height);
    float2 pixel_uv = pin.uv * screen_size;
    
   // float4 albedo_local = g_local_texture.Sample(g_sampler, pixel_uv + pin.uv_local - float2(0.5, 0.5)); //pin.uv_local);
    // albedo = lerp(albedo, albedo * albedo_local, 0.5f);
    albedo.w = 1.0;

    //// uv_local is expected in [0,1] across the quad; compute distance to center
    //float2 center = float2(0.5, 0.5);
    //float2 d = pin.uv_local - center;
    //float dist = length(d);
    //
    //// circle parameters (adjust radius/feather to taste)
    //const float radius = 0.5f;
    //const float feather = 0.05f; // smooth edge width
    //
    //// compute smooth circular mask (1.0 inside, 0.0 outside)
    //float mask = saturate(1.0f - smoothstep(radius - feather, radius, dist));
    //
    //// apply mask to alpha and color
    //albedo.rgb *= mask;
    //albedo.a *= mask;
    
    //albedo.a *= saturate(pin.life); // fade out based on life

    // discard fully transparent pixels early
    clip(albedo.a - 0.001f);

    pixel_out.color = albedo;
    pixel_out.emission = albedo * 1.5f;
    return pixel_out;
}
