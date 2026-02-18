#ifdef USE_VERTEX_OUT_POS
struct VertexOut
{
    float4 position_h : SV_POSITION;
};
#endif

#ifdef USE_VERTEX_OUT_POS_TEX
struct VertexOut
{
    float4 postion_h : SV_POSITION;
    float2 uv : TEXCOORD0;
};
#endif

// lighting...
#ifdef USE_VERTEX_OUT_POS_TEX_LIGHTING
struct VertexOut
{
    float4 postion_h : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 position_w : TEXCOORD1;
    float3 normal_w : NORMAL;
    float4 position_lw : TEXCOORD2; // TODO
    float4 position_v: TEXCOORD3; // TODO
    float4 normal_v : TEXCOORD3; // TODO
};
#endif

// skybox...
#ifdef USE_VERTEX_OUT_SKYBOX
struct VertexOut
{
    float4 postion_h : SV_POSTION;
    float4 postion_l : POSITION0; // local
};
#endif