#ifndef __BUFFER_MESH__
#define __BUFFER_MESH__
#include "buffer_camera.hlsli"

cbuffer BufferPerMesh : register(b2)
{
    float4x4 g_matrix_model;
    float4x4 g_matrix_model_inverse_transpose;
    // uv animaion
    float2 g_uv_offset;
    float2 g_uv_size;
};
#endif