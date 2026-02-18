 #include "vertex_out/vertex_out_base.hlsli"
 
 // materails
cbuffer ScreenBG : register(b0)
{
    float g_center_u;
    float g_center_v;
    float g_radius;
    float g_thickness;
    
    float g_time;
    float g_rotation;
    float g_stripe_density;
    float g_stripe_offset;

    float4 g_color_inside;
    float4 g_color_stripe_positive;
    float4 g_color_stripe_negative;
};

float CalculateSdRoundedX(float2 p, float w, float r)
{
    p = abs(p); // center symmetry
    return length(p - min(p.x + p.y, w) * 0.5f) - r;
}

float4 main(VertexOut pixel_in) : SV_TARGET
{
    float2 center = float2(g_center_u, g_center_v);
    float scale = 1.0f;
    float aspect_ratio = 1.0f; // TODO passin aspect ratio
    float2 p = pixel_in.uv - center;
    p.x = frac(p.x + 0.5f) - 0.5f;
    p.x *= aspect_ratio;
    p = p * 2.0f * scale;

    float d = CalculateSdRoundedX(p, g_radius, g_thickness);
    float step_center = step(0.0, d);

    float stripes = saturate(sin(g_stripe_density * d + g_time) + g_stripe_offset);

    float4 color_stripe = lerp(g_color_stripe_negative, g_color_stripe_positive, stripes);
    float4 color = lerp(g_color_inside, color_stripe, step_center);

    return color;
}