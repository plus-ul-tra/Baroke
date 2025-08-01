Texture2D g_MainTex : register(t0); 
Texture2D g_NoiseTex : register(t1); 
SamplerState g_Sampler : register(s0);

cbuffer TimeBuffer : register(b0)
{
    float time;
}

struct VS_OUTPUT // ���ؽ� ���̴� ��� ����ü�� ����
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // UV ��ũ��
    float2 noiseUV = Input.Tex + float2(time * 0.05, time * 0.03);
    float2 noise = g_NoiseTex.Sample(g_Sampler, noiseUV).rg;

   
    float distortionAmount = 0.02; // �ְ� ����
    float2 distortedUV = Input.Tex + (noise - 0.5) * distortionAmount;

  
    float4 baseColor = g_MainTex.Sample(g_Sampler, distortedUV);

    return baseColor;
}