struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float2 BarrelDistort(float2 uv, float distortionAmount)
{
    float2 centerUV = uv - 0.5;
    float dist = length(centerUV);

    float innerRadius = 0.0; // �ְ��� ���۵Ǵ� �߽ɿ����� �Ÿ� (���� 0)
    float outerRadius = 2.0; // �ְ��� �ִ�� ����Ǵ� �߽ɿ����� �Ÿ�
    
    float normalizedDist = smoothstep(innerRadius, outerRadius, dist);

    float distortFactor = 1.0 + distortionAmount * normalizedDist * normalizedDist;


    return (centerUV * distortFactor) + 0.5;
}

//float4 PSMain(VS_OUTPUT Input) : SV_Target
//{
//   // �跲 �ְ�
//    float distortionStrength = 0.5; //����
//    float2 distortedUV = BarrelDistort(Input.Tex, distortionStrength);
//    distortedUV = saturate(distortedUV);
//    float4 finalColor = g_Texture.Sample(g_Sampler, distortedUV);
    
//    // ��ĵ���� ȿ�� ����
//    float screenHeight = 300.0; // �е� 
//    float pixelY = Input.Tex.y * screenHeight;
//    float scanlineFactor = lerp(0.9, 1.0, fmod(floor(pixelY), 2.0));

//    finalColor.rgb *= scanlineFactor; // RGB ä�ο��� ����
    
//    // ���Ʈ (fix)
//    float2 uv = Input.Tex - 0.5; 
//    float dist = length(uv);
//    float vignetteStart = 0.63;
//    float vignetteEnd = 0.9;
//    float vignettePower = 10.0;
//    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
//    finalColor.rgb *= (1.0 - vignette * vignettePower);
    
//    return finalColor;
//}

// ���� ��ȯ ����
float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // 1. �跲 �ְ� ����
    float distortionStrength = 0.3;
    float2 distortedUV = BarrelDistort(Input.Tex, distortionStrength);
    distortedUV = saturate(distortedUV);

    // 2. ��ĵ���� ȿ�� (�ְ�� ��ǥ ���)
    float screenHeight = 300.0; // ��ĵ���� �е�
    float pixelY = distortedUV.y * screenHeight;
    float scanlineFactor = lerp(0.9, 1.0, fmod(floor(pixelY), 2.0));

    // 3. �ؽ�ó ���ø�
    float4 finalColor = g_Texture.Sample(g_Sampler, distortedUV);
    finalColor.rgb *= scanlineFactor;

    // 4. ���Ʈ ȿ��
    float2 uv = distortedUV - 0.5;
    float dist = length(uv);
    float vignetteStart = 0.65;
    float vignetteEnd = 0.9;
    float vignettePower = 10.0;
    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower);
    
    return finalColor;
}