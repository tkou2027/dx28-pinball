Texture2D<float4> g_texture_reflection : register(t0);
RWTexture2D<float4> g_texture_output : register(u0);

// TODO: refactor
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint2 pixel = id.xy;

    uint width, height;
    g_texture_reflection.GetDimensions(width, height);

    if (pixel.x >= width || pixel.y >= height)
    {
        return;
    }
    float4 center = g_texture_reflection.Load(int3(pixel, 0));
    // already valid reflection
    if (center.a > 0.1)
    {
        g_texture_output[pixel] = center;
        return;
    }
    // pair test (radius=1)
    float4 rL = g_texture_reflection.Load(int3(pixel + int2(-1, 0), 0));
    float4 rR = g_texture_reflection.Load(int3(pixel + int2(1, 0), 0));
    float4 rU = g_texture_reflection.Load(int3(pixel + int2(0, -1), 0));
    float4 rD = g_texture_reflection.Load(int3(pixel + int2(0, 1), 0));
    
    bool horizontalPair = (rL.a > 0.1 && rR.a > 0.1);
    bool verticalPair = (rU.a > 0.1 && rD.a > 0.1);
    
    if (!horizontalPair && !verticalPair)
    {
        g_texture_output[pixel] = center;
        return;
    }

    // radius=2 gather
    const int radius = 1;
    float3 color = 0;
    float weight = 0;
    [unroll]
    for (int y = -radius; y <= radius; y++)
    {
        [unroll]
        for (int x = -radius; x <= radius; x++)
        {
            if (x == 0 && y == 0)
                continue;

            int2 p = pixel + int2(x, y);

            if (p.x < 0 || p.y < 0 || p.x >= width || p.y >= height)
                continue;

            float4 r = g_texture_reflection.Load(int3(p, 0));

            if (r.a <= 0.1)
                continue;

            float dist = length(float2(x, y));
            float wgt = r.a / (1.0 + dist);

            color += r.rgb * wgt;
            weight += wgt;
        }
    }

    if (weight > 0)
    {
        float3 c = color / weight;
        c = c * 0.125;
        float v = saturate(weight * 0.125);
        g_texture_output[pixel] = float4(c, v);
    }
    else
    {
        g_texture_output[pixel] = center;
    }
}