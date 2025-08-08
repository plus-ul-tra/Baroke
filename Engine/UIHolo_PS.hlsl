
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
    if (baseColor.a < 0.01)           // 투명 영역은 통과
        return baseColor;


    float2 p = input.tex - float2(0.5, 0.5);
    float r = length(p); 
    float ang = atan2(p.y, p.x); 
    ang = frac(ang / (2 * 3.141592)); // 0~1 로 정규화


    float speed = 0.2; 
    float hue = frac(ang + time * speed); // 0~1
    float3 rainbow = HueToRGB(hue);


    float radialWave = sin(r * 40 - time * 5) * 0.5 + 0.5; 
    float pattern = radialWave * 0.7; 

    float3 holoColor = baseColor.rgb * (1.0 + rainbow * pattern * 1.5);
    return float4(holoColor, baseColor.a);
}