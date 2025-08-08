
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

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};
float3 HueToRGB(float h)
{
    float r = abs(h * 6 - 3) - 1;
    float g = 2 - abs(h * 6 - 2);
    float b = 2 - abs(h * 6 - 4);
    return saturate(float3(r, g, b));
}
float4 PSMain(VS_OUTPUT input) : SV_Target
{
    // 0) 원본 샘플?+?투명픽셀 early-out
    float4 baseColor = Texture.Sample(SamplerTexture, input.tex);
    if (baseColor.a < 0.01)
        return float4(0, 0, 0, 0);

    //------------------------------------------------------------
    // 1) 동심원 위치 계산 (r = 0→0.7 정도)
    //------------------------------------------------------------
    float2 p = input.tex - float2(0.5, 0.5);
    float r = length(p); // 반지름
    //      ↑ 화면 비율이 1:1 이 아니라면 p.x /= aspect 보정 추가

    //------------------------------------------------------------
    // 2) 진행하는 파동: 중심→밖으로 이동하는 링
    //------------------------------------------------------------
    float freq = 12.0; // 링 간격 (컷 수치를 늘리면 빽빽)
    float speed = 1.5; // 퍼져나가는 속도
    float wave = frac(r * freq - time * speed); // 0~1 sawtooth

    /* ‘보이는 영역’ 마스크
       - wave 가 0 근처(링 전면)에 있을 때만 α 상승
       - 0.0~0.15 구간만 부드럽게 나타나게 함 */
    float alpha = smoothstep(0.15, 0.00, wave);
    // 0.0~0.15   → 1 → 0
    // 나머지(0.15~1) → 0

    //------------------------------------------------------------
    // 3) 최종 색 : RGB 그대로, 알파 = 마스크
    //------------------------------------------------------------
    return float4(baseColor.rgb , baseColor.a);
}