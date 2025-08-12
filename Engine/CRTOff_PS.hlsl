Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// 0=������ ����, 1=������ ����
cbuffer TimeBuffer : register(b0)
{
    float time;
    float deltaTime;
    float2 padding;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // �ְ�/��ĵ���� ��ǥ ���
    float4 col = g_Texture.Sample(g_Sampler, Input.Tex);

    // 2) time ���൵ p(0��1)
    
    const float TOTAL = 1.0; // ��ü ���� �ð�(��)
    float p = saturate(time / TOTAL); // 0��1

    //    (B) �ݺ� ����: �ֱ������� �ݺ��Ϸ��� �Ʒ� �� �ٷ� ��ü
    // float p = frac(time / TOTAL);          // �ֱ� �ݺ�

    // 3) �� �ܰ� ����: ���� ����(0~0.7), ���� ����(0.7~1.0)
    float p1 = saturate(p / 0.70); // 0~0.7 ����
    float p2 = saturate((p - 0.70) / 0.30); // 0.7~1.0 ����

    // 4) �߽� ���� ��ǥ
    float2 c = Input.Tex - 0.5;

    // 5) ������(����). �ּҰ��� �ξ� ���� ��ǥ�� ���� 0�� ���� �ʰ� ��
    float yScale = max(0.003, lerp(1.0, 0.003, p1)); // ���ΰ� ���� ����
    float xScale = max(0.003, lerp(1.0, 0.003, p2)); // �� ���� ���� ����

    float2 uvCollapse = float2(c.x * xScale, c.y * yScale) + 0.5;
    uvCollapse = saturate(uvCollapse);

    // 6) ��ҵ� ȭ�� ����� + ���
    col = g_Texture.Sample(g_Sampler, uvCollapse);
    float dim = 1.0 - p; // p=1�� �� ���� ����
    col.rgb *= dim;

    // 7) ���Ρ��� �۷ο�(���� ����: ���� ������ �� ��� ����)
    float lineWidth = lerp(0.25, 0.003, p1);
    float lineGlow = exp(-pow(abs(c.y) / lineWidth, 2.0));
    float dotWidth = lerp(0.20, 0.003, p2);
    float dotGlow = exp(-pow(length(c) / dotWidth, 2.0));
    float glow = lerp(lineGlow, dotGlow, p2);
    col.rgb += glow * 1.5 * (0.8 + 0.2 * (1.0 - dim)); // ���̶���Ʈ

    return col;
}
