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

    // 투명 영역
    if (baseColor.a < 0.01)
        return baseColor;

    // 홀로그램 패턴
    float wave = sin((input.tex.y + time * 0.5) * 30.0) * 0.5 + 0.5; // 세로줄
 


    float pattern = wave * 0.6 /*+ scanline * 0.4*/;


    float hue = frac(time * 0.2 + pattern * 0.2);
    float3 rainbow = HueToRGB(hue);


    float3 holoColor = baseColor.rgb * (1.0 + rainbow * pattern * 1.5);

    return float4(holoColor, baseColor.a);
}


