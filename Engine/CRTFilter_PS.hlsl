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

    float innerRadius = 0.0; // 왜곡이 시작되는 중심에서의 거리 (보통 0)
    float outerRadius = 2.0; // 왜곡이 최대로 적용되는 중심에서의 거리
    
    float normalizedDist = smoothstep(innerRadius, outerRadius, dist);

    float distortFactor = 1.0 + distortionAmount * normalizedDist * normalizedDist;


    return (centerUV * distortFactor) + 0.5;
}

//float4 PSMain(VS_OUTPUT Input) : SV_Target
//{
//   // 배럴 왜곡
//    float distortionStrength = 0.5; //강도
//    float2 distortedUV = BarrelDistort(Input.Tex, distortionStrength);
//    distortedUV = saturate(distortedUV);
//    float4 finalColor = g_Texture.Sample(g_Sampler, distortedUV);
    
//    // 스캔라인 효과 적용
//    float screenHeight = 300.0; // 밀도 
//    float pixelY = Input.Tex.y * screenHeight;
//    float scanlineFactor = lerp(0.9, 1.0, fmod(floor(pixelY), 2.0));

//    finalColor.rgb *= scanlineFactor; // RGB 채널에만 적용
    
//    // 비네트 (fix)
//    float2 uv = Input.Tex - 0.5; 
//    float dist = length(uv);
//    float vignetteStart = 0.63;
//    float vignetteEnd = 0.9;
//    float vignettePower = 10.0;
//    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
//    finalColor.rgb *= (1.0 - vignette * vignettePower);
    
//    return finalColor;
//}
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
    
    //시간 값에 따른 UV 변화, 글리치 강도 조절 가능.
    // Rgb
    float2 glitchOffset = float2(0.001, 0.005); // UV 이동
    float4 texR = g_Texture.Sample(g_Sampler, distortedUV + glitchOffset); // R 채널
    float4 texG = g_Texture.Sample(g_Sampler, distortedUV - glitchOffset); // G 채널
    float4 texB = g_Texture.Sample(g_Sampler, distortedUV); // B 채널 그대로

    // 스캔라인이 홀수일 때 RGB 채널에 색상 섞기
    float4 glitchColor = float4(texR.r, texG.g, texB.b, 1.0);
    float glitchBlend = scanlineToggle * 0.4; // 글리치 강도 (0~0.2 추천)
    
    
    
    // 3. 텍스처 샘플링
    float4 baseColor = g_Texture.Sample(g_Sampler, distortedUV);
    float4 finalColor = lerp(baseColor, glitchColor, glitchBlend);
    finalColor.rgb *= scanlineFactor;

    // 4. 비네트 효과
    float2 uv = distortedUV - 0.5;
    float dist = length(uv);
    float vignetteStart = 0.65;
    float vignetteEnd = 0.9;
    float vignettePower = 10.0;
    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower);
    
    return finalColor;
}