struct VS_INPUT
{
    float3 Pos : POSITION; // ���� ��ġ (3D)
    float2 Tex : TEXCOORD0; // �ؽ�ó ��ǥ (2D)
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; // ���� Ŭ�� ���� ��ġ
    float2 Tex : TEXCOORD0; // �ؽ�ó ��ǥ (������ ��)
};



VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT Output;

    Output.Pos = float4(Input.Pos, 1.0f); // �Է� ��ġ�� ���� Ŭ�� ���� ��ġ�� ��� (Ǯ��ũ�� �����)
    Output.Tex = Input.Tex; // �ؽ�ó ��ǥ�� �ȼ� ���̴��� ����
    
    return Output;
}
