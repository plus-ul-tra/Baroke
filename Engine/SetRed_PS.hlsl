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

float2 LocalToAtlasUV(float2 local)
{
    float2 texSize = float2(textureWidth, textureHeight);
    float2 minPx = float2(sourceRectX, sourceRectY);
    float2 sizePx = float2(sourceRectWidth, sourceRectHeight);

    float2 minUV = (minPx + 0.5) / texSize;
    float2 maxUV = (minPx + sizePx - 0.5) / texSize;
    return lerp(minUV, maxUV, saturate(local));
}

float4 PSMain(VS_OUTPUT input) : SV_Target
{

    float2 localUV = input.tex;


    float2 atlasUV = LocalToAtlasUV(localUV);


    float4 baseColor = Texture.Sample(SamplerTexture, atlasUV);
    if (baseColor.a < 0.01)
        return baseColor;


    float3 glowColor = float3(1.0, 0.2, 0.0);
    float minGlowStrength = 0.3;

    float flicker = (sin(time * 8.0) + 1.0) * 0.5; // 0~1
    flicker = pow(flicker, 2.0);

  
    float glowStrength = minGlowStrength + flicker * (1.0 - minGlowStrength * 2.0);

 
    float3 finalColor = lerp(baseColor.rgb, glowColor, glowStrength);


    return float4(finalColor, baseColor.a);
}