#include "vertex_out/vertex_out_fullscreen.hlsli"

// full screen triangle
// (-1, 1)________ (3, 1)
//        |   |  /
// (-1,-1)|___|/ (1, -1)
//        |  /
// (-1,-3)|/
VertexOut main(uint vertexID : SV_VertexID)
{
    VertexOut vertexOut;
	
    float2 grid = float2((vertexID << 1) & 2, vertexID & 2); // [0, 1, 2] => [(0,0), (2,0), (0,2)]
    float2 xy = grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f); // => [(-1,1), (3,1), (-1,-3)]
    vertexOut.uv = grid;
    vertexOut.postion_h = float4(xy, 1.0f, 1.0f);
    return vertexOut;
}