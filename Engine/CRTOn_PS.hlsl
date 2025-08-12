Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// 0=¿ÏÀüÈ÷ ÄÑÁü, 1=¿ÏÀüÈ÷ ²¨Áü
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
   
    const float TOTAL = 1.0; 
    float p = saturate(time / TOTAL); 

 
    float p2 = saturate(p / 0.30); 
    float p1 = saturate((p - 0.30) / 0.70); 

 
    float2 c = Input.Tex - 0.5;

    const float minScale = 0.003;
    float xScale = max(minScale, lerp(minScale, 1.0, p2)); 
    float yScale = max(minScale, lerp(minScale, 1.0, p1)); 

    float2 uvExpand = float2(c.x * xScale, c.y * yScale) + 0.5;
    uvExpand = saturate(uvExpand);


    float4 col = g_Texture.Sample(g_Sampler, uvExpand);

    col.rgb *= p;

 
    float lineWidth = lerp(0.003, 0.25, p1);
    float dotWidth = lerp(0.003, 0.20, p2);     
    float lineGlow = exp(-pow(abs(c.y) / lineWidth, 2.0));
    float dotGlow = exp(-pow(length(c) / dotWidth, 2.0));
    float glow = lerp(dotGlow, lineGlow, p1);
    col.rgb += glow * 1.2 * (1.0 - p); 

    return col;
}