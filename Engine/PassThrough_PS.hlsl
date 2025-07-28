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
    
//    //return float4(0.0f, 0.0f, 1.0f, 1.0f); 테스트용
//}

//스캔 라인

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

    // 스캔라인 효과: 픽셀의 Y좌표를 사용하여 홀수/짝수 줄마다 밝기 조절
    // Input.Tex.y는 0.0에서 1.0 범위입니다. 여기에 가상의 해상도 높이를 곱
    // 예를 들어, 480은 CRT 화면의 일반적인 수직 해상도
    float screenHeight = 480.0; // 이 값을 바꾸면서 스캔라인 밀도를 조절해 보세요.
    float pixelY = Input.Tex.y * screenHeight;

    // fmod(pixelY, 2.0)는 짝수 픽셀 줄(0.0)과 홀수 픽셀 줄(1.0)을 구분합니다.
    // lerp(어두운_값, 밝은_값, t) : t가 0이면 어두운_값, 1이면 밝은_값
    // 여기서는 짝수 줄을 1.0(원래 밝기), 홀수 줄을 0.8(어둡게)로 만듭니다.
    float scanlineFactor = lerp(0.8, 1.0, fmod(floor(pixelY), 2.0));

    sampledColor.rgb *= scanlineFactor; // RGB 채널에만 적용하여 색상 유지

    return sampledColor;
}

// vignette




