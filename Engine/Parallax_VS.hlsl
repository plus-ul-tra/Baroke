

cbuffer MouseEffectConstants : register(b1) // ���ο� ��� ���� ����
{
    float2 MouseInfluence; // X: -1.0 ~ 1.0, Y: -1.0 ~ 1.0
    float Padding[2];
};

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

// VS_OUTPUT: �ȼ� ���̴��� ����
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; // ���� ȭ�� ��ġ
    float2 Tex : TEXCOORD0; // �ؽ�ó UV
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT Output;

    // --- 1. �⺻ ���� ��ġ ---
    // Input.Position�� �̹� NDC��� �״�� ���, �ƴ϶�� ����-��-�������� ��ȯ �ʿ�
    float4 finalPos = Input.Position;

    // --- 2. ���콺�� ���� ���� �̵��� ��� ---
    float effectStrength = 0.05; // ��ü���� ����� ���� (���� �ʿ�)
    float parallaxFactor = 0.5; // �� ���̾��� ���� ���� (��: ����� 0.2, ������ 0.8)

    float2 offset = -MouseInfluence * effectStrength * parallaxFactor;

    // --- 3. ���� ��ġ�� ������ ���� ---
    // �� �������� finalPos.xy�� ���� ���Ͽ� ������ �̵���ŵ�ϴ�.
    // (�� ���ô� Input.Position�� �̹� ȭ�� ���� ��ǥ�� �����ϴٰ� ����)
    finalPos.x += offset.x;
    finalPos.y += offset.y;

    Output.Pos = finalPos;
    Output.Tex = Input.TexCoord;

    return Output;
}