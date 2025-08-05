cbuffer TextureAtlasCBuffer : register(b1)
{
    float textureWidth;
    float textureHeight;
    float sourceRectX;
    float sourceRectY;
    float sourceRectWidth;
    float sourceRectHeight;
  
};

SamplerState SamplerTexture : register(s0);
Texture2D Texture : register(t0);

struct VS_OUTPUT // 버텍스 쉐이더 출력 구조체와 동일
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

cbuffer TimeBuffer : register(b0)
{
    float time;
    float deltaTime;
    float2 padding;
}

float4 PSMain(VS_OUTPUT input) : SV_Target
{

    float4 baseColor = Texture.Sample(SamplerTexture, input.tex);


    if (baseColor.a < 0.01)
        return baseColor;

// 기본 붉은색 기운 (최소치)
    float3 redGlow = float3(1.0, 0.2, 0.2); // 주황빛 빨강
    float minGlowStrength = 0.3; 


    float flicker = (sin(time * 8.0) + 1.0) * 0.5; // 0~1 범위
    flicker = pow(flicker, 2.0); 

    float glowStrength = minGlowStrength + flicker * (1.0 - minGlowStrength*2);


    float3 finalColor = lerp(baseColor.rgb, redGlow, glowStrength);

    return float4(finalColor, baseColor.a);
}