
Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);


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

// 해시(의사 난수) 함수들 ? 텍스처 없이 노이즈 생성
float hash11(float x)
{
    return frac(sin(x * 12.9898) * 43758.5453);
}
float hash12(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}


float2 BarrelDistort(float2 uv, float distortionAmount)
{
    float2 c = uv - 0.5;
    float d = length(c);
    float t = smoothstep(0.0, 2.0, d);
    float k = 1.0 + distortionAmount * t * t;
    return c * k + 0.5;
}

float4 PSMain(VS_OUTPUT In) : SV_Target
{
  
    float2 uv0 = In.Tex;

  
    float2 uv = BarrelDistort(uv0, 0.30);
    uv = saturate(uv);


    float screenHeight = 300.0;
    float row = uv.y * screenHeight;
    float rowFloor = floor(row);


    float scanToggle = fmod(rowFloor, 2.0);
    float scanFactor = lerp(0.90, 1.0, scanToggle);

    float lineNoise = hash12(float2(rowFloor, floor(time * 60.0)));
    float lineJitterAmp = 0.001; // 줄이면 안정, 늘리면 강한 지터
    uv.x += (lineNoise - 0.5) * 2.0 * lineJitterAmp;


    float2 grid = float2(32.0, 18.0); 
    float2 id = floor(uv * grid);
    float blockRnd = hash12(id + floor(time * 4.0));

    float2 blockOffset = 0.0;
    if (blockRnd > 0.97)
    { // 발동 확률
        float a = (hash12(id * 1.37 + time) - 0.5) * 2.0;
        float b = (hash12(id * 3.11 + time * 0.7) - 0.5) * 2.0;
        float blockAmp = 0.003; // 블록 오프셋 강도
        blockOffset = float2(a, b) * blockAmp;
    }
    uv += blockOffset;
    uv = saturate(uv);

    float dropRnd = hash11(rowFloor + floor(time * 25.0));
    float dropout = step(0.995, dropRnd); // 확률

    float dropoutDarken = lerp(1.0, 0.6, dropout);
    scanFactor *= dropoutDarken;


    float2 glitchOffset = float2(0.001, 0.005);


    float burst = step(0.98, hash11(floor(time * 3.0))); // 1.0 됨
    float burstScale = lerp(1.0, 2.0, burst);
    float2 chromaOffset = glitchOffset * burstScale;

    float4 texR = g_Texture.Sample(g_Sampler, saturate(uv + chromaOffset));
    float4 texG = g_Texture.Sample(g_Sampler, saturate(uv - chromaOffset));
    float4 texB = g_Texture.Sample(g_Sampler, uv);

    float4 base = g_Texture.Sample(g_Sampler, uv);
    float4 glitchColor = float4(texR.r, texG.g, texB.b, 1.0);

 
    float glitchBlend = scanToggle * 0.20;
    float4 col = lerp(base, glitchColor, glitchBlend);


    col.rgb *= scanFactor;

    float2 v = uv - 0.5;
    float dist = length(v);
    float vignette = smoothstep(0.65, 0.90, dist);
    col.rgb *= (1.0 - vignette * 10.0);

    return col;
}
