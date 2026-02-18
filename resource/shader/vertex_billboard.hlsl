struct VS_OUT
{
    uint instance_id : TEXCOORD0;
};

VS_OUT main(uint instance_id : SV_InstanceID)
{
    VS_OUT output;
    output.instance_id = instance_id; // to gs
    return output;
}