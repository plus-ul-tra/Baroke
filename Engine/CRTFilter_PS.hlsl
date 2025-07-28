struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// ���ο� ��� ���� ���� (b0 �������Ϳ� ���ε�)
// C++�� ����ü�� ����
cbuffer TimeConstants : register(b0)
{
    float Time; // C++���� �Ѿ�� �ð� ��
    float Padding0; // 16����Ʈ ������ ���� �е�
    float Padding1;
    float Padding2;
};


float random(float2 uv)
{
    // uv�� Time�� ���Ͽ� �� ������ �ٸ� ���� ������ ����
    return frac(sin(dot(uv + Time, float2(12.9898, 78.233))) * 43758.5453123);
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // 1. �⺻ ���� ���ø� (���� �̹���)
    float4 finalColor = g_Texture.Sample(g_Sampler, Input.Tex);

    // 2. ��ĵ���� ȿ�� ����
    float screenHeight = 480.0; // ��ĵ���� �е� (�� ���� �ٲ㺸����)
    float pixelY = Input.Tex.y * screenHeight;
    float scanlineFactor = lerp(0.8, 1.0, fmod(floor(pixelY), 2.0)); // ¦�� ���� ���, Ȧ�� ���� ��Ӱ�
    finalColor.rgb *= scanlineFactor; // RGB ä�ο��� ��ĵ���� ����

    // 3. ���Ʈ ȿ�� ����
    float2 uv = Input.Tex - 0.5; // �߾��� (0,0)����
    float dist = length(uv); // �߾����κ����� �Ÿ�

    float vignetteStart = 0.4; // ���Ʈ ���� �Ÿ�
    float vignetteEnd = 0.9; // ���Ʈ �ִ� �Ÿ�
    float vignettePower = 1.0; // ���Ʈ ����

    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower); // ���Ʈ ���� (��ĵ���� ����� ���� �����)

    // 4. ������/�ʸ� 
    float noiseStrength = 0.05; // ������ ���� ���� (0.0~1.0)

    float noise = random(Input.Tex * 200.0); // 100.0�� ���ؼ� 

    finalColor.rgb += (noise - 0.5) * noiseStrength * 2.0; // ����� ���� (��ĵ����, ���Ʈ ����� ����)

    // ���� Ŭ���� 0.0~1.0 ����
    finalColor.rgb = saturate(finalColor.rgb);

    return finalColor;
}