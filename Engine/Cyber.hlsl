
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

cbuffer TimeBuffer : register(b0)
{
    float time;
    float deltaTime;
    float2 padding;
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};
float3 HueToRGB(float h)
{
    float r = abs(h * 6 - 3) - 1;
    float g = 2 - abs(h * 6 - 2);
    float b = 2 - abs(h * 6 - 4);
    return saturate(float3(r, g, b));
}
float4 PSMain(VS_OUTPUT input) : SV_Target
{

    float4 baseColor = Texture.Sample(SamplerTexture, input.tex);
    if (baseColor.a < 0.01)
        return float4(0, 0, 0, 0);


    float2 p = input.tex - float2(0.5, 0.5);
    float r = length(p); // 반지름



    float freq = 12.0; // 링 간격 (컷 수치를 늘리면 빽빽)
    float speed = 1.5; // 퍼져나가는 속도
    float wave = frac(r * freq - time * speed); // 0~1 sawtooth


    float alpha = smoothstep(0.15, 0.00, wave);

    return float4(baseColor.rgb , baseColor.a);
}