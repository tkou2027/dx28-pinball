// material
struct MaterialOutline
{
    float4 color;
    float width;
    float _padding_width_1;
    float _padding_width_2;
    float _padding_width_3;
};

// material buffer
cbuffer CelSetting : register(b2)
{
    MaterialOutline material_outline;
};

// vertex to pixel
struct VertexOutOutline
{
    float4 position : SV_POSITION;
};