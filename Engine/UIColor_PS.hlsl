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

cbuffer Color: register(b0)
{
    float4 targetColor;
    float4 paddingColor;
};

cbuffer Time : register(b2)
{
    float time;
    float deltaTime;
    float2 padding;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT input) : SV_Target
{
    float2 uv;
    uv.x = (sourceRectX + input.tex.x * sourceRectWidth) / textureWidth;
    uv.y = (sourceRectY + input.tex.y * sourceRectHeight) / textureHeight;
    
    float4 baseColor = Texture.Sample(SamplerTexture, uv);
    //float wave = sin((input.tex.y + time * 0.5) * 30.0) * 0.5 + 0.5;
    baseColor.rgb *= targetColor.rgb;
    baseColor.a *= targetColor.a;
    return baseColor;
    
}
//float4 PSMain(VS_OUTPUT input) : SV_Target
//{

//    float2 uv;
//    uv.x = (sourceRectX + input.tex.x * sourceRectWidth) / textureWidth;
//    uv.y = (sourceRectY + input.tex.y * sourceRectHeight) / textureHeight;

//    float4 baseColor = Texture.Sample(SamplerTexture, uv);

//    float t = input.tex.y; // 선형
//    t = frac(input.tex.y + time * 1.6);

//    // 4) 보간:  t=0 → 원본,  t=1 → targetColor 필터 적용
//    float3 tinted = lerp(baseColor.rgb, baseColor.rgb * targetColor.rgb, t);

//    float outA = lerp(baseColor.a, baseColor.a * targetColor.a, t);

//    return float4(tinted, outA);
 
//}