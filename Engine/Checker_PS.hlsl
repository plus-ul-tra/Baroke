cbuffer TextureAtlasCBuffer : register(b1)
{
	float textureWidth;
	float textureHeight;
	float sourceRectX;
	float sourceRectY;
	float sourceRectWidth;
	float sourceRectHeight;
  
};

Texture2D Texture : register(t0);
Texture2D nextTexture : register(t1);

SamplerState SamplerTexture : register(s0);


cbuffer TimeBuffer : register(b0)
{
	float time;
	float deltaTime;
	float2 padding;
}
#define GRID 17        
#define EDGE 1.0
struct VS_OUTPUT // 버텍스 쉐이더 출력 구조체와 동일
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT input) : SV_Target
{
	float2 uvA;
	uvA.x = (sourceRectX + input.tex.x * sourceRectWidth) / textureWidth;
	uvA.y = (sourceRectY + input.tex.y * sourceRectHeight) / textureHeight;

   
	float2 uvB = input.tex; // nextTexture 가 통짜 이미지라고 가정

	float4 colA = Texture.Sample(SamplerTexture, uvA);
	float4 colB = nextTexture.Sample(SamplerTexture, uvB);	
    float2 cell = floor(input.tex * GRID); // (x, y)
    float diag = (GRID - 1 - cell.x) + cell.y; // 0 … 2*GRID-2
    float diagMax = (GRID - 1) * 2.0;

   // float thresh = saturate(time/1.5) * ((GRID - 1) * 2);
   //
   // // 부드러운 전환
   // float mask = smoothstep(thresh - EDGE, thresh + EDGE, diag);
    float diagNorm = diag / diagMax; // 0 … 1
    float edgeNorm = EDGE / diagMax; // 폭 보정
    float mask = smoothstep(time - edgeNorm,
                                 time,
                                 diagNorm);
  
    return lerp(colB, colA, mask);
}