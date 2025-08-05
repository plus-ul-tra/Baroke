Texture2D g_MainTex : register(t0); 
Texture2D g_NoiseTex1 : register(t1); 
Texture2D g_NoiseTex2 : register(t2);

SamplerState g_Sampler : register(s0);

cbuffer TimeBuffer : register(b0)
{ 
    float time;
    float deltaTime;
    float2 padding;
}

struct VS_OUTPUT 
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
     // 노이즈 
    float2 scrollSpeed1 = float2(0.05, 0.0);
    float2 noiseUV1 = frac(Input.Tex + scrollSpeed1 * time);
    float2 noise1 = g_NoiseTex1.Sample(g_Sampler, noiseUV1).rg;

    // 노이즈 2
    float2 scrollSpeed2 = float2(0.0, 0.05);
    float2 noiseUV2 = frac(Input.Tex + scrollSpeed2 * time);
    float2 noise2 = g_NoiseTex2.Sample(g_Sampler, noiseUV2).rg;

    // 노이즈 혼합 
    float2 mixedNoise = lerp(noise1, noise2, 0.5);

    // 왜곡 적용
    float distortionAmount = 0.05;
    float2 distortedUV = saturate(Input.Tex + (mixedNoise - 0.5) * distortionAmount);

    float4 baseColor = g_MainTex.Sample(g_Sampler, distortedUV);

    // 색상 변화 
    float rShift = (sin(time / 2) + 1.0) * 0.5;
    float gShift = (sin(time / 2 + 2.0) + 1.0) * 0.5;
    float bShift = (sin(time / 2 + 4.0) + 1.0) * 0.5;
  
    baseColor.rgb *= float3(rShift, gShift, bShift);

    return baseColor;
}