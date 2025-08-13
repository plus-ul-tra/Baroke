Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};



float2 BarrelDistort(float2 uv, float distortionAmount)
{
    float2 centerUV = uv - 0.5;
    float dist = length(centerUV);

    float innerRadius = 0.0; 
    float outerRadius = 2.0; 
    
    float normalizedDist = smoothstep(innerRadius, outerRadius, dist);

    float distortFactor = 1.0 + distortionAmount * normalizedDist * normalizedDist;


    return (centerUV * distortFactor) + 0.5;
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // 배럴 왜곡 적용
    float distortionStrength = 0.3;
    float2 distortedUV = BarrelDistort(Input.Tex, distortionStrength);
    distortedUV = saturate(distortedUV);

    // 스캔라인 효과 (왜곡된 좌표 기반)
    float screenHeight = 300.0; // 스캔라인 밀도
    float pixelY = distortedUV.y * screenHeight;
    float scanlineToggle = fmod(floor(pixelY), 2.0); //
    float scanlineFactor = lerp(0.9, 1.0, fmod(floor(pixelY), 2.0));
    

    float2 glitchOffset = float2(0.001, 0.005); // UV 이동
    float4 texR = g_Texture.Sample(g_Sampler, distortedUV + glitchOffset); // R 채널
    float4 texG = g_Texture.Sample(g_Sampler, distortedUV - glitchOffset); 
    float4 texB = g_Texture.Sample(g_Sampler, distortedUV); 

    float4 glitchColor = float4(texR.r, texG.g, texB.b, 1.0);
    float glitchBlend = scanlineToggle * 0.2; // 글리치 강도 
    
    
    
    // 텍스처 샘플링
    float4 baseColor = g_Texture.Sample(g_Sampler, distortedUV);
    float4 finalColor = lerp(baseColor, glitchColor, glitchBlend);
    finalColor.rgb *= scanlineFactor;

    // 비네트
    float2 uv = distortedUV - 0.5;
    float dist = length(uv);
    float vignetteStart = 0.65;
    float vignetteEnd = 0.9;
    float vignettePower = 10.0;
    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower);

    
    return finalColor;
}