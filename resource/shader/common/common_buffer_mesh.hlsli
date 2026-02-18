// constant buffer ====
struct DirectionalLight
{
    float3 direction;
    float _padding_direction;
};

cbuffer BufferPerFrame : register(b0)
{
    // view
    column_major float4x4 matrix_view_proj;
    column_major float4x4 matrix_view_proj_no_transform;
    column_major float4x4 matrix_view;
    // camera
    float3 camera_position;
    float _camera_position_padding;
    // lighting
    DirectionalLight directional_light;
    float4x4 matrix_directional_light_view_proj;
};

cbuffer BufferPerMesh : register(b1)
{
    float4x4 matrix_model;
    float4x4 matrix_model_inverse_transpose;
};