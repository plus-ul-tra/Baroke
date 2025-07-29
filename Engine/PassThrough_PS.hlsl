struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; 
    float2 Tex : TEXCOORD0;
};


Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    return g_Texture.Sample(g_Sampler, Input.Tex);
}