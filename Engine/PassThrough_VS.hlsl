struct VS_INPUT
{
    float3 Pos : POSITION; 
    float2 Tex : TEXCOORD0; 
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; 
    float2 Tex : TEXCOORD0; 
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT Output;

    Output.Pos = float4(Input.Pos, 1.0f); 
    Output.Tex = Input.Tex; 
    
    return Output;
}
