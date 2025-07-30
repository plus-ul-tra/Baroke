cbuffer ObjectTransformCBuffer : register(b0)
{
    row_major matrix World;
    row_major matrix View;
    row_major matrix Projection;
};

// 버텍스 쉐이더 입력 (SpriteVertex 구조체와 일치)
struct VS_INPUT
{
    float3 position : POSITION; // 3D 위치 (x, y, z)
    float2 texcoord : TEXCOORD; // 텍스처 좌표 (u, v)
};

// 버텍스 쉐이더 출력 (픽셀 쉐이더 입력)
struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // 최종 화면 공간 위치
    float2 tex : TEXCOORD; // 픽셀 쉐이더로 전달될 텍스처 좌표 (보간됨)
};

VS_OUTPUT SpriteVS(VS_INPUT input)
{
    VS_OUTPUT output;

    // 월드, 뷰, 프로젝션 행렬을 적용하여 최종 위치 계산
    output.pos = mul(float4(input.position, 1.0f), World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    // 입력된 텍스처 좌표를 그대로 픽셀 쉐이더로 전달 (보간될 것임)
    output.tex = input.texcoord;

    return output;
}