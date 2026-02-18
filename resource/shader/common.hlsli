// lights ====
struct DirectionalLight
{
    float3 direction;
    float _padding_direction;
};

// constant buffer ====
cbuffer BufferPerFrame : register(b0)
{
    // view
    float4x4 matrix_view_proj;
    float4x4 matrix_view_proj_no_transform;
    // camera
    float3 camera_position;
    float _camera_position_padding;
    // light
    DirectionalLight directional_light;
    float4x4 matrix_directional_light_view_proj;
};

cbuffer BufferPerMesh : register(b1)
{
    float4x4 matrix_model;
    float4x4 matrix_model_inverse_transpose;
};

// vertex ====
struct VertexInBase
{
    float3 position : Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VertexInDynamic
{
    float3 position : Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    int4   bone_index : TEXCOORD1;
    float4 bone_weight : TEXCOORD2;
};
