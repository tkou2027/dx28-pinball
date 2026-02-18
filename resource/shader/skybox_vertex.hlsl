#include "vertex_in/vertex_in_mesh_static.hlsli"
#include "common/buffer_camera.hlsli"
#include "common_skybox.hlsli"

VertexOutSkybox main(VertexIn vertexIn)
{
    VertexOutSkybox vertex_out;

    float4x4 view_no_translation = g_matrix_view;
    view_no_translation._41 = 0.0f;
    view_no_translation._42 = 0.0f;
    view_no_translation._43 = 0.0f;
    float4x4 matrix_view_proj_no_transform = mul(view_no_translation, g_matrix_proj);
    
    float4 posH = mul(float4(vertexIn.position * 2.0f, 1.0f), matrix_view_proj_no_transform);
    
    vertex_out.posH = posH.xyww;
    vertex_out.posL = vertexIn.position * 2.0f;

    return vertex_out;
}