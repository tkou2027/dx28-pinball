struct VertexOut
{
    float4 postion_h : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 position_w : TEXCOORD1;
    float3 normal_w : NORMAL;
    float4 position_lw : TEXCOORD2; // TODO
    float4 position_v: TEXCOORD3; // TODO
    float4 normal_v : TEXCOORD4; // TODO
};