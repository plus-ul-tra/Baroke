Texture2D g_MainTex : register(t0); 
Texture2D g_NoiseTex : register(t1); 
SamplerState g_Sampler : register(s0);

cbuffer TimeBuffer : register(b0)
{ 
    float time;
    float deltaTime;
    float2 padding;
}

struct VS_OUTPUT // ���ؽ� ���̴� ��� ����ü�� ����
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // UV ��ũ��
    float2 scrollSpeed = float2(0.05, 0.03);
    float2 noiseUV = Input.Tex + (scrollSpeed * time); // noise ����ũ��
    noiseUV = frac(noiseUV);
    float2 noise = g_NoiseTex.Sample(g_Sampler, noiseUV).rg;

   
    float distortionAmount = 0.05; // �ְ� ����
    float2 distortedUV = Input.Tex + (noise - 0.5) * distortionAmount;
    distortedUV = saturate(distortedUV);
    float4 baseColor = g_MainTex.Sample(g_Sampler, distortedUV);
    
    // ���� ��ȭ
    float rShift = (sin(time) + 1.0) * 0.5; // 0~1
    float gShift = (sin(time + 2.0) + 1.0) * 0.5; 
    float bShift = (sin(time + 4.0) + 1.0) * 0.5;
  
     baseColor.rgb *= float3(rShift, gShift, bShift);

    return baseColor;
}