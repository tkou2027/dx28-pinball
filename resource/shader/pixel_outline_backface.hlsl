#include "common_outline.hlsli"

#define USE_VERTEX_OUT_POS
#include "common/common_vertex_out.hlsli"

struct PixelOut
{
    float4 color : SV_Target;
};

PixelOut main(VertexOut pixelIn)
{
    PixelOut pixelOut;

    pixelOut.color = material_outline.color;
    
    return pixelOut;
}