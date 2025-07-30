cbuffer ObjectTransformCBuffer : register(b0)
{
    row_major matrix World;
    row_major matrix View;
    row_major matrix Projection;
};

// ���ؽ� ���̴� �Է� (SpriteVertex ����ü�� ��ġ)
struct VS_INPUT
{
    float3 position : POSITION; // 3D ��ġ (x, y, z)
    float2 texcoord : TEXCOORD; // �ؽ�ó ��ǥ (u, v)
};

// ���ؽ� ���̴� ��� (�ȼ� ���̴� �Է�)
struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // ���� ȭ�� ���� ��ġ
    float2 tex : TEXCOORD; // �ȼ� ���̴��� ���޵� �ؽ�ó ��ǥ (������)
};

VS_OUTPUT SpriteVS(VS_INPUT input)
{
    VS_OUTPUT output;

    // ����, ��, �������� ����� �����Ͽ� ���� ��ġ ���
    output.pos = mul(float4(input.position, 1.0f), World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    // �Էµ� �ؽ�ó ��ǥ�� �״�� �ȼ� ���̴��� ���� (������ ����)
    output.tex = input.texcoord;

    return output;
}