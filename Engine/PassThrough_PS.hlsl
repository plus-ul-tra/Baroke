struct VS_OUTPUT
{

    float4 Pos : SV_POSITION; // 정점 셰이더에서 전달된 클립 공간 좌표
    float2 Tex : TEXCOORD0; // 텍스처 좌표
};

// 셰이더 리소스 및 샘플러
Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// 픽셀 셰이더: 텍스처를 그대로 화면에 출력

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    return g_Texture.Sample(g_Sampler, Input.Tex);
}