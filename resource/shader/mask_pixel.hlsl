#include "common_mask.hlsli"

struct PixelOut
{
    float4 color : SV_Target;
};

PixelOut main(VertexOutMask pixelIn)
{
    PixelOut pixelOut;
    // pixelOut.color = materail_mask.mask_value;
    pixelOut.color = float4(1.0, 1.0, 1.0, 1.0);
    return pixelOut;
}