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
    float outerRadius = 1.2; // �ְ��� �ִ�� ����Ǵ� �߽ɿ����� �Ÿ� (0.5�� ȭ�� �𼭸�)
    
    float normalizedDist = smoothstep(innerRadius, outerRadius, dist);

    float distortFactor = 1.0 + distortionAmount * normalizedDist * normalizedDist;


    return (centerUV * distortFactor) + 0.5;
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
   
    float distortionStrength = 0.3;
    float2 distortedUV = BarrelDistort(Input.Tex, distortionStrength);

    
    distortedUV = saturate(distortedUV);

   
    float4 finalColor = g_Texture.Sample(g_Sampler, distortedUV);
    
    // ��ĵ���� ȿ�� ����
 
    float screenHeight = 380.0; // �е� 
    float pixelY = Input.Tex.y * screenHeight;
    float scanlineFactor = lerp(0.9, 1.0, fmod(floor(pixelY), 2.0));

    finalColor.rgb *= scanlineFactor; // RGB ä�ο��� ����
    
    float2 uv = Input.Tex - 0.5; // ���Ʈ
    float dist = length(uv);
    float vignetteStart = 0.4;
    float vignetteEnd = 0.9;
    float vignettePower = 1.0;
    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower);
    
    
    return finalColor;
}