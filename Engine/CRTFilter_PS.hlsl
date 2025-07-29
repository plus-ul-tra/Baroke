struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// 새로운 상수 버퍼 정의 (b0 레지스터에 바인딩)
// C++의 구조체와 매핑
cbuffer TimeConstants : register(b0)
{
    float Time; // C++에서 넘어올 시간 값
    float Padding0; // 16바이트 정렬을 위한 패딩
    float Padding1;
    float Padding2;
};


float random(float2 uv)
{
    // uv에 Time을 더하여 매 프레임 다른 난수 시퀀스 생성
    return frac(sin(dot(uv + Time, float2(12.9898, 78.233))) * 43758.5453123);
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // 1. 기본 색상 샘플링 (원본 이미지)
    float4 finalColor = g_Texture.Sample(g_Sampler, Input.Tex);

    // 2. 스캔라인 효과 적용
    float screenHeight = 480.0; // 스캔라인 밀도 (이 값을 바꿔보세요)
    float pixelY = Input.Tex.y * screenHeight;
    float scanlineFactor = lerp(0.8, 1.0, fmod(floor(pixelY), 2.0)); // 짝수 줄은 밝게, 홀수 줄은 어둡게
    finalColor.rgb *= scanlineFactor; // RGB 채널에만 스캔라인 적용

    // 3. 비네트 효과 적용
    float2 uv = Input.Tex - 0.5; // 중앙을 (0,0)으로
    float dist = length(uv); // 중앙으로부터의 거리

    float vignetteStart = 0.4; // 비네트 시작 거리
    float vignetteEnd = 0.9; // 비네트 최대 거리
    float vignettePower = 1.0; // 비네트 강도

    float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
    finalColor.rgb *= (1.0 - vignette * vignettePower); // 비네트 적용 (스캔라인 적용된 색상에 덮어씌움)

    // 4. 노이즈/필름 
    float noiseStrength = 0.05; // 노이즈 강도 조절 (0.0~1.0)

    float noise = random(Input.Tex * 200.0); // 100.0을 곱해서 

    finalColor.rgb += (noise - 0.5) * noiseStrength * 2.0; // 노이즈를 더함 (스캔라인, 비네트 적용된 색상에)

    // 색상 클램핑 0.0~1.0 범위
    finalColor.rgb = saturate(finalColor.rgb);

    return finalColor;
}