#include "common/buffer_camera.hlsli"
#include "vertex_out/vertex_out_sprite.hlsli"

struct GS_IN
{
    uint instance_id : TEXCOORD0;
};

struct BillboardInstanceData
{
    float3 position;
    float roattion;
    float2 size;
    float2 uv_offset;
    float2 uv_size;
};

StructuredBuffer<BillboardInstanceData> g_billboard_data : register(t0);

[maxvertexcount(4)]
void main(
	point GS_IN input[1],
	inout TriangleStream<VertexOut> output_stream
)
{
    BillboardInstanceData data = g_billboard_data[input[0].instance_id];
    
    // float3 up = float3(0.0f, 1.0f, 0.0f);
    // float3 look = g_view_position_w - data.position;
    // look.y = 0.0f;
    // look = normalize(look);
    // float3 right = cross(up, look);
    
    float3 right = float3(g_matrix_view[0][0], g_matrix_view[1][0], g_matrix_view[2][0]);
    float3 up = float3(g_matrix_view[0][1], g_matrix_view[1][1], g_matrix_view[2][1]);

    float3 center = data.position;
    float2 halfSize = data.size * 0.5f;

    float3 v[4];
    v[0] = center + (-right * halfSize.x) + (-up * halfSize.y);
    v[1] = center + (-right * halfSize.x) + (up * halfSize.y);
    v[2] = center + (right * halfSize.x) + (-up * halfSize.y);
    v[3] = center + (right * halfSize.x) + (up * halfSize.y); 

    float2 uv[4];
    uv[0] = float2(data.uv_offset.x, data.uv_offset.y + data.uv_size.y);
    uv[1] = float2(data.uv_offset.x, data.uv_offset.y);
    uv[2] = float2(data.uv_offset.x + data.uv_size.x, data.uv_offset.y + data.uv_size.y);
    uv[3] = float2(data.uv_offset.x + data.uv_size.x, data.uv_offset.y);

    // triangle strip
    VertexOut output;
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        output.position_h = mul(float4(v[i], 1.0f), g_matrix_view_proj);
        output.uv = uv[i];
        output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        output_stream.Append(output);
    }
}