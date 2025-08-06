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


struct VS_OUTPUT // ���ؽ� ���̴� ��� ����ü�� ����
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 baseColor = Texture.Sample(SamplerTexture, input.tex);

    // ���� �ȼ��� �н�
    if (baseColor.a < 0.01)
        return baseColor;

  
    float brightness = dot(baseColor.rgb, float3(0.299, 0.587, 0.114));

    float progress = saturate(time - input.tex.x);

    float adjusted = pow(1.0 - brightness, 0.8); 
    float fadeFactor = saturate(progress * adjusted * 1.5);

    // ���� �� �������� ����
    float3 finalColor = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), fadeFactor);

    return float4(finalColor, baseColor.a);
}