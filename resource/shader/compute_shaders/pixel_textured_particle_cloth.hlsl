Texture2D g_particle_texture : register(t0);
Texture2D g_local_texture : register(t1);
SamplerState g_sampler : register(s0);

struct PSIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float life : TEXCOORD1;
    float3 normal_w : NOTMAL0;
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

    float4 albedo = g_particle_texture.Sample(g_sampler, pin.uv);
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
