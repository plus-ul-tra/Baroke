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
struct VS_OUTPUT // ���ؽ� ���̴� ��� ����ü�� ����
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT input) : SV_Target
{
	float2 uvA;
	uvA.x = (sourceRectX + input.tex.x * sourceRectWidth) / textureWidth;
	uvA.y = (sourceRectY + input.tex.y * sourceRectHeight) / textureHeight;

    // 2) ����(B) �ؽ�ó�� UV ? ��¥��� �״��, ��Ʋ�󽺸� ���� ������� ����
	float2 uvB = input.tex; // nextTexture �� ��¥ �̹������ ����
    // �ʿ� �� uvB ���� scale-offset ������� �����ϸ� �ȴ�

	float4 colA = Texture.Sample(SamplerTexture, uvA);
	float4 colB = nextTexture.Sample(SamplerTexture, uvB);

    // 3) �� �ε��� ���
	float2 cell = floor(input.tex * GRID); // [0, GRID-1]
	int index = (int) cell.x + (int) cell.y * GRID; // 0~GRID��-1
	int totalCell = GRID * GRID;

    // 4) ���൵(threshold) ? time �� [0,1]
	float thresh = saturate(time) * totalCell; // 0~GRID��

    // 5) �������� �ƿ���
	float mask = smoothstep(thresh - EDGE, thresh + EDGE, (float) index);

    // 6) A��B ���� (mask==0 �� B, mask==1 �� A)
	return lerp(colB, colA, mask);
}