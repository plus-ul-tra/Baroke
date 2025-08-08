
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
    // 0) ���� ����?+?�����ȼ� early-out
    float4 baseColor = Texture.Sample(SamplerTexture, input.tex);
    if (baseColor.a < 0.01)
        return float4(0, 0, 0, 0);

    //------------------------------------------------------------
    // 1) ���ɿ� ��ġ ��� (r = 0��0.7 ����)
    //------------------------------------------------------------
    float2 p = input.tex - float2(0.5, 0.5);
    float r = length(p); // ������
    //      �� ȭ�� ������ 1:1 �� �ƴ϶�� p.x /= aspect ���� �߰�

    //------------------------------------------------------------
    // 2) �����ϴ� �ĵ�: �߽ɡ������ �̵��ϴ� ��
    //------------------------------------------------------------
    float freq = 12.0; // �� ���� (�� ��ġ�� �ø��� ����)
    float speed = 1.5; // ���������� �ӵ�
    float wave = frac(r * freq - time * speed); // 0~1 sawtooth

    /* �����̴� ������ ����ũ
       - wave �� 0 ��ó(�� ����)�� ���� ���� �� ���
       - 0.0~0.15 ������ �ε巴�� ��Ÿ���� �� */
    float alpha = smoothstep(0.15, 0.00, wave);
    // 0.0~0.15   �� 1 �� 0
    // ������(0.15~1) �� 0

    //------------------------------------------------------------
    // 3) ���� �� : RGB �״��, ���� = ����ũ
    //------------------------------------------------------------
    return float4(baseColor.rgb , baseColor.a);
}