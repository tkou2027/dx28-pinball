#ifndef __BUFFER_CAMERA__
#define __BUFFER_CAMERA__
cbuffer BufferPerProjection : register(b0)
{
    float4x4 g_matrix_proj;
    float4x4 g_matrix_proj_inverse;
};

cbuffer BufferPerView : register(b1)
{
    float4x4 g_matrix_view;
    float4x4 g_matrix_view_inverse;
    float4x4 g_matrix_view_proj;
    float4x4 g_matrix_view_proj_inverse;
    float3 g_view_position_w;
    float _padding_view_position_w;
};
#endif