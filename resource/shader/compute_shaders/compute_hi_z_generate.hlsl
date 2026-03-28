Texture2D<float> g_texture_in : register(t0);
RWTexture2D<float> g_texture_out : register(u0);

cbuffer ScreenParams : register(b0)
{
    uint g_width;
    uint g_height;
    uint g_width_in;
    uint g_height_in;
};

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= g_width || id.y >= g_height)
    {
        return;
    }
    
    // pervious level uv
    uint2 src_uv_0 = id.xy * 2;
    uint2 src_uv_1 = src_uv_0 + uint2(1, 1);
    src_uv_0 = min(src_uv_0, uint2(g_width_in - 1, g_height_in - 1));
    src_uv_1 = min(src_uv_1, uint2(g_width_in - 1, g_height_in - 1));

    // sample 2x2 region
    float d_0 = g_texture_in.Load(uint3(src_uv_0.x, src_uv_0.y, 0)).r;
    float d_1 = g_texture_in.Load(uint3(src_uv_1.x, src_uv_0.y, 0)).r;
    float d_2 = g_texture_in.Load(uint3(src_uv_0.x, src_uv_1.y, 0)).r;
    float d_3 = g_texture_in.Load(uint3(src_uv_1.x, src_uv_1.y, 0)).r;

    // get min depth
    float d_min = min(min(d_0, d_1), min(d_2, d_3));

    g_texture_out[id.xy] = d_min;
}