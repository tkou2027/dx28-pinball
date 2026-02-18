struct VertexOut
{
    float4 position_h : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 position_w : TEXCOORD1;
    float3 normal_w : NORMAL;
    float4 tangent_w : TANGENT;
    float4 position_v : TEXCOORD2;
    float4 color : COLOR;
    nointerpolation uint instance_id : TEXCOORD3;
};