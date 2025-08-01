

cbuffer MouseEffectConstants : register(b1) // 새로운 상수 버퍼 슬롯
{
    float2 MouseInfluence; // X: -1.0 ~ 1.0, Y: -1.0 ~ 1.0
    float Padding[2];
};

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

// VS_OUTPUT: 픽셀 쉐이더로 전달
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; // 최종 화면 위치
    float2 Tex : TEXCOORD0; // 텍스처 UV
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT Output;

    // Input.Position이 이미 NDC라면 그대로 사용
    float4 finalPos = Input.Position;


    float effectStrength = 0.05; // 전체적인 기울임 강도 (조정 필요)
    float parallaxFactor = 0.5; // 이 레이어의 시차 강도 (예: 배경은 0.2, 전경은 0.8)

    float2 offset = -MouseInfluence * effectStrength * parallaxFactor;


    finalPos.x += offset.x;
    finalPos.y += offset.y;

    Output.Pos = finalPos;
    Output.Tex = Input.TexCoord;

    return Output;
}