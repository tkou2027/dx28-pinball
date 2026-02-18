// material
struct MaterialMask
{
    float mask_value;
    float _padding_1;
    float _padding_2;
    float _padding_3;
};

cbuffer MaskSetting : register(b2)
{
    MaterialMask materail_mask;
}

struct VertexOutMask
{
    float4 position : SV_POSITION;
};