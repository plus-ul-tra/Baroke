Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// 0=완전히 켜짐, 1=완전히 꺼짐
cbuffer TimeBuffer : register(b0)
{
    float time;
    float deltaTime;
    float2 padding;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // 왜곡/스캔라인 좌표 사용
    float4 col = g_Texture.Sample(g_Sampler, Input.Tex);

    // 2) time 진행도 p(0→1)
    
    const float TOTAL = 1.0; // 전체 꺼짐 시간(초)
    float p = saturate(time / TOTAL); // 0→1

    //    (B) 반복 꺼짐: 주기적으로 반복하려면 아래 한 줄로 대체
    // float p = frac(time / TOTAL);          // 주기 반복

    // 3) 두 단계 진행: 세로 수축(0~0.7), 가로 수축(0.7~1.0)
    float p1 = saturate(p / 0.70); // 0~0.7 구간
    float p2 = saturate((p - 0.70) / 0.30); // 0.7~1.0 구간

    // 4) 중심 기준 좌표
    float2 c = Input.Tex - 0.5;

    // 5) 스케일(수축). 최소값을 두어 샘플 좌표가 완전 0이 되지 않게 함
    float yScale = max(0.003, lerp(1.0, 0.003, p1)); // 세로가 먼저 납작
    float xScale = max(0.003, lerp(1.0, 0.003, p2)); // 그 다음 가로 수축

    float2 uvCollapse = float2(c.x * xScale, c.y * yScale) + 0.5;
    uvCollapse = saturate(uvCollapse);

    // 6) 축소된 화면 재샘플 + 디밍
    col = g_Texture.Sample(g_Sampler, uvCollapse);
    float dim = 1.0 - p; // p=1일 때 완전 암전
    col.rgb *= dim;

    // 7) 라인→점 글로우(선택 사항: 끄고 싶으면 이 블록 삭제)
    float lineWidth = lerp(0.25, 0.003, p1);
    float lineGlow = exp(-pow(abs(c.y) / lineWidth, 2.0));
    float dotWidth = lerp(0.20, 0.003, p2);
    float dotGlow = exp(-pow(length(c) / dotWidth, 2.0));
    float glow = lerp(lineGlow, dotGlow, p2);
    col.rgb += glow * 1.5 * (0.8 + 0.2 * (1.0 - dim)); // 하이라이트

    return col;
}
