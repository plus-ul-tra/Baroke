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

// 1. ����� �ܼ��� ���������� ��ȯ
// 2. ������ texture �ڿ����� ��ȯ
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 baseColor = Texture.Sample(SamplerTexture, input.tex);

    // ���� �ȼ��� �н�
    if (baseColor.a < 0.01)
        return baseColor;

    // ��� ��� (����ϼ��� ���� 1�� �����)
    float brightness = dot(baseColor.rgb, float3(0.299, 0.587, 0.114));

    // ���ʿ��� ���������� �����ϴ� �׶��̼�
    float progress = saturate(time - input.tex.x);
    // time�� �����Ҽ��� progress�� ���ʿ��� ���������� 0��1

    // ����ϼ��� �� ���� ���ϵ��� ��
    //float fadeFactor = saturate(progress * (1.0-brightness));
    float adjusted = pow(1.0 - brightness, 0.8); // 0.5 �� ��Ʈ�� ���� ���� �κ� �����, ��ο� �κ� �����
    float fadeFactor = saturate(progress * adjusted * 1.5);

    // ���� �� �������� ����
    float3 finalColor = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), fadeFactor);

    return float4(finalColor, baseColor.a);
}