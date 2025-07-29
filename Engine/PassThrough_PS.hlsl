struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float2 BarrelDistort(float2 uv, float distortionAmount)
{
    // uv 좌표를 -0.5 ~ 0.5 범위로 정규화하여 중앙을 (0,0)으로 만듭니다.
    float2 centerUV = uv - 0.5;

    // 중앙으로부터의 거리 (피타고라스 정리)
    float dist = length(centerUV);

    // 왜곡 계수 계산
    float distortFactor = 1.0 + distortionAmount * dist * dist;

    // 왜곡된 uv 좌표 계산
    return (centerUV * distortFactor) + 0.5;
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
   
    float distortionStrength = 0.1; // 왜곡 강도 조절 (0.05 ~ 0.2 정도에서 테스트)
    float2 distortedUV = BarrelDistort(Input.Tex, distortionStrength);

    // 왜곡된 UV 좌표가 텍스처 범위를 벗어나지 않도록 클램프합니다.
    distortedUV = saturate(distortedUV);

    // 왜곡된 UV로 원본 색상을 샘플링합니다.
    float4 finalColor = g_Texture.Sample(g_Sampler, distortedUV);
    
    // --- 2. 스캔라인 효과 적용 ---
    // 스캔라인은 왜곡된 이미지 위에 덮어씌우는 효과이므로,
    // 원본 텍스처 좌표(Input.Tex.y)를 사용하여 계산하는 것이 더 자연스럽습니다.
    float screenHeight = 380.0; // 스캔라인 밀도 조절
    float pixelY = Input.Tex.y * screenHeight;
    float scanlineFactor = lerp(0.9, 1.0, fmod(floor(pixelY), 2.0));

    finalColor.rgb *= scanlineFactor; // RGB 채널에만 적용
    
    float2 uv = Input.Tex - 0.5; // 비네트도 원본 UV로 계산 (왜곡된 UV를 사용하면 비네트도 왜곡됨)
    float dist = length(uv);
    float vignetteStart = 0.4;
    float vignetteEnd = 0.9;
    float vignettePower = 1.0;
    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower);
    
    
    return finalColor; // 최종적으로 효과가 모두 적용된 색상 반환
}

// vignette




