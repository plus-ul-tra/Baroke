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
#define GRID 16          
#define EDGE 0.5
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

    // 2) 다음(B) 텍스처용 UV ? 통짜라면 그대로, 아틀라스면 같은 방식으로 보정
	float2 uvB = input.tex; // nextTexture 가 통짜 이미지라고 가정
    // 필요 시 uvB 역시 scale-offset 방식으로 보정하면 된다

	float4 colA = Texture.Sample(SamplerTexture, uvA);
	float4 colB = nextTexture.Sample(SamplerTexture, uvB);

    // 3) 셀 인덱스 계산
	float2 cell = floor(input.tex * GRID); // [0, GRID-1]
	int index = (int) cell.x + (int) cell.y * GRID; // 0~GRID²-1
	int totalCell = GRID * GRID;

    // 4) 진행도(threshold) ? time ∈ [0,1]
	float thresh = saturate(time) * totalCell; // 0~GRID²

    // 5) 스무스한 컷오프
	float mask = smoothstep(thresh - EDGE, thresh + EDGE, (float) index);

    // 6) A→B 보간 (mask==0 ⇒ B, mask==1 ⇒ A)
	return lerp(colB, colA, mask);
}