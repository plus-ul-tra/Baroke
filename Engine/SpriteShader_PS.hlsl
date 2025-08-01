cbuffer TextureAtlasCBuffer : register(b1)
{
    float textureWidth;
    float textureHeight;
    float sourceRectX;
    float sourceRectY;
    float sourceRectWidth;
    float sourceRectHeight;
    // float padding[2]; // 패딩은 C++에서만 필요, 쉐이더에서는 구조체 멤버만 선언
};

// 텍스처 샘플러 (register(s0)는 샘플러 슬롯 0)
SamplerState SamplerTexture : register(s0); 
// 텍스처 리소스 (register(t0)는 텍스처 슬롯 0)
Texture2D Texture : register(t0); 

struct VS_OUTPUT // 버텍스 쉐이더 출력 구조체와 동일
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD; 
};

float4 SpritePS(VS_OUTPUT input) : SV_TARGET
{
    // 텍스처 아틀라스 내의 정확한 텍셀 좌표를 계산
    // input.tex는 0~1 범위의 쿼드 UV를 의미.
    // 이를 sourceRect 정보를 사용하여 아틀라스 내의 실제 픽셀 좌표로 변환합니다.

    float finalU = (sourceRectX + input.tex.x * sourceRectWidth) / textureWidth;
    // Y축 뒤집기 (필요하다면. D3D는 Y가 위로 증가하고, 이미지는 아래로 증가할 수 있음)
    // input.tex.y가 0.0 (top) ~ 1.0 (bottom)으로 오고,
    // sourceRectY가 텍스처의 top을 기준으로 한다면, 이대로 괜찮습니다.
    // 만약 이미지가 Direct2D처럼 Y가 아래로 증가하는 UV를 따르는데,
    // D3D 기본이 Y가 위로 증가하는 것으로 설정되었다면 뒤집어야 합니다.
    // 보통 쉐이더에서 Texcoord.y = 1.0f - Texcoord.y; 처럼 처리합니다.
    float finalV = (sourceRectY + input.tex.y * sourceRectHeight) / textureHeight; 
    
    // 텍스처를 샘플링하고 최종 색상 반환
    return Texture.Sample(SamplerTexture, float2(finalU, finalV));
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
   
}