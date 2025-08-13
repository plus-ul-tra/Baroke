cbuffer TextureAtlasCBuffer : register(b1)
{
    float textureWidth;
    float textureHeight;
    float sourceRectX;
    float sourceRectY;
    float sourceRectWidth;
    float sourceRectHeight;
  
};
SamplerState SmplerTexture : register(s0);
Texture2D Texture : register(t0);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 color = Texture.Sample(SmplerTexture, input.tex);

    float gray = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));

    return float4(gray, gray, gray, color.a);
}