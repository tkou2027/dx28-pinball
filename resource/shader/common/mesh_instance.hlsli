struct MeshInstance
{
    float4x4 matrix_model;
    float4x4 matrix_model_inverse_transpose;
    float2 uv_offset;
    float2 uv_size;
};