// material
struct MaterialCel
{
    float shader_mode; // TODO: this should be removed
    float ramp_offset; // TODO: this should be sampled from texture
    float rim_radius;
    float rim_intensity;
    float receive_shadow;
    float use_alter_texture; // TODO: this should be something else
    float _padding_1;
    float _padding_2;
};

// material buffer
cbuffer CelSetting : register(b2)
{
    MaterialCel material_cel;
};

// vertex to pixel
struct VertexOutCel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal_world : NORMAL;
    float4 position_world : TEXCOORD1;
    float4 position_dir_light : TEXCOORD2;
};