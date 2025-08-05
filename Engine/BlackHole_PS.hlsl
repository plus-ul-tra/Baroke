Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

// 중심입력 버퍼 필요

cbuffer TimeBuffer : register(b1)
{
	float time;
	float deltaTime;
	float2 padding;
}

cbuffer Pos : register(b0)
{
	 // 블랙홀 중심 좌표
    float centerX;
    float centerY;
    float2 pad;
};

// 기본 배럴 왜곡
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

// 블랙홀 왜곡
float2 BlackHoleDistort(float2 uv, float2 holeCenter, float radius, float strength)
{
    float2 dir = holeCenter - uv;
    float dist = length(dir);

    if (dist < radius)
    {
        // 중심에 가까울수록 당김 강도 커짐
        float pull = strength * (1.0 - dist / radius);

        float angle = time * (1.0 - dist / radius); //time

 
        float s = sin(angle);
        float c = cos(angle);

        float2 rotatedDir = float2(
            dir.x * c - dir.y * s,
            dir.x * s + dir.y * c
        );

        uv = holeCenter - rotatedDir * (1.0 - pull);
    }

    return uv;
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    // 1. 배럴 왜곡
	float distortionStrength = 0.3;
	float2 uv = BarrelDistort(Input.Tex, distortionStrength);
	uv = saturate(uv);

    // 2. 블랙홀 왜곡 
    float2 blackHoleCenter = float2(centerX, centerY); // 좌표로 지정  변환 필요
	float blackHoleRadius = 0.15; // 반경
	float blackHoleStrength = 0.10; // 강도
	uv = BlackHoleDistort(uv, blackHoleCenter, blackHoleRadius, blackHoleStrength);
	uv = saturate(uv);

    // 3. 스캔라인 & 글리치
	float screenHeight = 300.0;
	float pixelY = uv.y * screenHeight;
	float scanlineToggle = fmod(floor(pixelY), 2.0);
	float scanlineFactor = lerp(0.9, 1.0, scanlineToggle);

	float2 glitchOffset = float2(0.001, 0.005);
	float4 texR = g_Texture.Sample(g_Sampler, uv + glitchOffset);
	float4 texG = g_Texture.Sample(g_Sampler, uv - glitchOffset);
	float4 texB = g_Texture.Sample(g_Sampler, uv);

	float4 glitchColor = float4(texR.r, texG.g, texB.b, 1.0);
	float glitchBlend = scanlineToggle * 0.2;

	float4 baseColor = g_Texture.Sample(g_Sampler, uv);
	float4 finalColor = lerp(baseColor, glitchColor, glitchBlend);
	finalColor.rgb *= scanlineFactor;

    // 4. 비네트
	float2 vignetteUV = uv - 0.5;
	float dist = length(vignetteUV);
	float vignetteStart = 0.65;
	float vignetteEnd = 0.9;
	float vignettePower = 10.0;
	float vignette = smoothstep(vignetteStart, vignetteEnd, dist);
	finalColor.rgb *= (1.0 - vignette * vignettePower);

	return finalColor;
}
