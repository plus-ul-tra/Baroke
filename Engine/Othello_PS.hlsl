cbuffer TextureAtlasCBuffer : register(b1)
{
    float textureWidth;
    float textureHeight;
    float sourceRectX;
    float sourceRectY;
    float sourceRectWidth;
    float sourceRectHeight;
  
};

SamplerState SamplerTexture : register(s0);
Texture2D Texture : register(t0);

cbuffer TimeBuffer : register(b0)
{
    float time;
    float deltaTime;
    float2 padding;
}


struct VS_OUTPUT // 버텍스 쉐이더 출력 구조체와 동일
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

// 1. 흰색을 단순히 검은색으로 변환
// 2. 검은돌 texture 자원으로 변환
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 baseColor = Texture.Sample(SamplerTexture, input.tex);

    // 투명 픽셀은 패스
    if (baseColor.a < 0.01)
        return baseColor;

    // 밝기 계산 (흰색일수록 값이 1에 가까움)
    float brightness = dot(baseColor.rgb, float3(0.299, 0.587, 0.114));

    // 왼쪽에서 오른쪽으로 진행하는 그라데이션
    float progress = saturate(time - input.tex.x);
    // time이 증가할수록 progress가 왼쪽에서 오른쪽으로 0→1

    // 흰색일수록 더 빨리 변하도록 곱
    //float fadeFactor = saturate(progress * (1.0-brightness));
    float adjusted = pow(1.0 - brightness, 0.8); // 0.5 ↓ 루트를 씌워 밝은 부분 영향↑, 어두운 부분 영향↓
    float fadeFactor = saturate(progress * adjusted * 1.5);

    // 원본 → 검정으로 보간
    float3 finalColor = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), fadeFactor);

    return float4(finalColor, baseColor.a);
}