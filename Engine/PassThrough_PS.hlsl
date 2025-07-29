//// FullScreenTexture_PS.hlsl
// passThrough
//struct VS_OUTPUT
//{
//    float4 Pos : SV_POSITION;
//    float2 Tex : TEXCOORD0;
//};

//Texture2D g_Texture : register(t0);
//SamplerState g_Sampler : register(s0);

//float4 PSMain(VS_OUTPUT Input) : SV_Target
//{
//    return g_Texture.Sample(g_Sampler, Input.Tex);
    
//    //return float4(0.0f, 0.0f, 1.0f, 1.0f); �׽�Ʈ��
//}

//��ĵ ����

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    float4 sampledColor = g_Texture.Sample(g_Sampler, Input.Tex);

    // ��ĵ���� ȿ��: �ȼ��� Y��ǥ�� ����Ͽ� Ȧ��/¦�� �ٸ��� ��� ����
    // Input.Tex.y�� 0.0���� 1.0 �����Դϴ�. ���⿡ ������ �ػ� ���̸� ��
    // ���� ���, 480�� CRT ȭ���� �Ϲ����� ���� �ػ�
    float screenHeight = 480.0; // �� ���� �ٲٸ鼭 ��ĵ���� �е��� ������ ������.
    float pixelY = Input.Tex.y * screenHeight;

    // fmod(pixelY, 2.0)�� ¦�� �ȼ� ��(0.0)�� Ȧ�� �ȼ� ��(1.0)�� �����մϴ�.
    // lerp(��ο�_��, ����_��, t) : t�� 0�̸� ��ο�_��, 1�̸� ����_��
    // ���⼭�� ¦�� ���� 1.0(���� ���), Ȧ�� ���� 0.8(��Ӱ�)�� ����ϴ�.
    float scanlineFactor = lerp(0.8, 1.0, fmod(floor(pixelY), 2.0));

    sampledColor.rgb *= scanlineFactor; // RGB ä�ο��� �����Ͽ� ���� ����

    return sampledColor;
}

// vignette




