cbuffer TextureAtlasCBuffer : register(b1)
{
    float textureWidth;
    float textureHeight;
    float sourceRectX;
    float sourceRectY;
    float sourceRectWidth;
    float sourceRectHeight;
    // float padding[2]; // �е��� C++������ �ʿ�, ���̴������� ����ü ����� ����
};

// �ؽ�ó ���÷� (register(s0)�� ���÷� ���� 0)
SamplerState SamplerTexture : register(s0); 
// �ؽ�ó ���ҽ� (register(t0)�� �ؽ�ó ���� 0)
Texture2D Texture : register(t0); 

struct VS_OUTPUT // ���ؽ� ���̴� ��� ����ü�� ����
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD; 
};

float4 SpritePS(VS_OUTPUT input) : SV_TARGET
{
    // �ؽ�ó ��Ʋ�� ���� ��Ȯ�� �ؼ� ��ǥ�� ���
    // input.tex�� 0~1 ������ ���� UV�� �ǹ�.
    // �̸� sourceRect ������ ����Ͽ� ��Ʋ�� ���� ���� �ȼ� ��ǥ�� ��ȯ�մϴ�.

    float finalU = (sourceRectX + input.tex.x * sourceRectWidth) / textureWidth;
    // Y�� ������ (�ʿ��ϴٸ�. D3D�� Y�� ���� �����ϰ�, �̹����� �Ʒ��� ������ �� ����)
    // input.tex.y�� 0.0 (top) ~ 1.0 (bottom)���� ����,
    // sourceRectY�� �ؽ�ó�� top�� �������� �Ѵٸ�, �̴�� �������ϴ�.
    // ���� �̹����� Direct2Dó�� Y�� �Ʒ��� �����ϴ� UV�� �����µ�,
    // D3D �⺻�� Y�� ���� �����ϴ� ������ �����Ǿ��ٸ� ������� �մϴ�.
    // ���� ���̴����� Texcoord.y = 1.0f - Texcoord.y; ó�� ó���մϴ�.
    float finalV = (sourceRectY + input.tex.y * sourceRectHeight) / textureHeight; 
    
    // �ؽ�ó�� ���ø��ϰ� ���� ���� ��ȯ
    return Texture.Sample(SamplerTexture, float2(finalU, finalV));
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
   
}