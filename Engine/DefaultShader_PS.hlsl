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

float4 SpritePS(VS_OUTPUT input) : SV_TARGET
{


    float finalU = (sourceRectX + input.tex.x * sourceRectWidth) / textureWidth;

    float finalV = (sourceRectY + input.tex.y * sourceRectHeight) / textureHeight; 
    
  
    return Texture.Sample(SamplerTexture, float2(finalU, finalV));
 
   
}