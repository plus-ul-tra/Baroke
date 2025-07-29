struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float2 BarrelDistort(float2 uv, float distortionAmount)
{
    // uv ��ǥ�� -0.5 ~ 0.5 ������ ����ȭ�Ͽ� �߾��� (0,0)���� ����ϴ�.
    float2 centerUV = uv - 0.5;

    // �߾����κ����� �Ÿ� (��Ÿ��� ����)
    float dist = length(centerUV);

    // �ְ� ��� ���
    float distortFactor = 1.0 + distortionAmount * dist * dist;

    // �ְ�� uv ��ǥ ���
    return (centerUV * distortFactor) + 0.5;
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
   
    float distortionStrength = 0.1; // �ְ� ���� ���� (0.05 ~ 0.2 �������� �׽�Ʈ)
    float2 distortedUV = BarrelDistort(Input.Tex, distortionStrength);

    // �ְ�� UV ��ǥ�� �ؽ�ó ������ ����� �ʵ��� Ŭ�����մϴ�.
    distortedUV = saturate(distortedUV);

    // �ְ�� UV�� ���� ������ ���ø��մϴ�.
    float4 finalColor = g_Texture.Sample(g_Sampler, distortedUV);
    
    // --- 2. ��ĵ���� ȿ�� ���� ---
    // ��ĵ������ �ְ�� �̹��� ���� ������ ȿ���̹Ƿ�,
    // ���� �ؽ�ó ��ǥ(Input.Tex.y)�� ����Ͽ� ����ϴ� ���� �� �ڿ��������ϴ�.
    float screenHeight = 380.0; // ��ĵ���� �е� ����
    float pixelY = Input.Tex.y * screenHeight;
    float scanlineFactor = lerp(0.9, 1.0, fmod(floor(pixelY), 2.0));

    finalColor.rgb *= scanlineFactor; // RGB ä�ο��� ����
    
    float2 uv = Input.Tex - 0.5; // ���Ʈ�� ���� UV�� ��� (�ְ�� UV�� ����ϸ� ���Ʈ�� �ְ��)
    float dist = length(uv);
    float vignetteStart = 0.4;
    float vignetteEnd = 0.9;
    float vignettePower = 1.0;
    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower);
    
    
    return finalColor; // ���������� ȿ���� ��� ����� ���� ��ȯ
}

// vignette




