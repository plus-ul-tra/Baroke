cbuffer ObjectTransformCBuffer : register(b0)
{
    row_major matrix World;
    row_major matrix View;
    row_major matrix Projection;
};

cbuffer TimeBuffer : register(b1)
{
    float deltaTime; 
};


struct VS_INPUT
{
    float3 position : POSITION; // 3D ��ġ (x, y, z)
    float2 texcoord : TEXCOORD; // �ؽ�ó ��ǥ (u, v)
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // ���� ȭ�� ���� ��ġ
    float2 tex : TEXCOORD; // �ȼ� ���̴��� ���޵� �ؽ�ó ��ǥ (������)
};

VS_OUTPUT SpriteVS(VS_INPUT input)
{
    VS_OUTPUT output;

    // ����, ��, �������� ��� ����
    output.pos = mul(float4(input.position, 1.0f), World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);


    output.tex = input.texcoord;

    return output;
}