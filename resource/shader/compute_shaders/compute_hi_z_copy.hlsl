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
    g_texture_out[id.xy] = g_texture_in.Load(uint3(id.xy, 0)).r;
}