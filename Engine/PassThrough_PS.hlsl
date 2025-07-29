struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; // ���� ���̴����� ���޵� Ŭ�� ���� ��ǥ
    float2 Tex : TEXCOORD0; // �ؽ�ó ��ǥ
};

// ���̴� ���ҽ� �� ���÷�
Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// �ȼ� ���̴�: �ؽ�ó�� �״�� ȭ�鿡 ���
float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    return g_Texture.Sample(g_Sampler, Input.Tex);
}