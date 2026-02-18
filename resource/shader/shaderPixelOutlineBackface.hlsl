#include "common.hlsli"
#include "common_outline.hlsli"

struct PixelOut
{
    float4 color : SV_Target;
};

PixelOut main(VertexOutOutline pixelIn)
{
    PixelOut pixelOut;

    pixelOut.color = material_outline.color;
    
    return pixelOut;
}