struct VS_INPUT
{
    float3 Pos : POSITION; // 정점 위치 (3D)
    float2 Tex : TEXCOORD0; // 텍스처 좌표 (2D)
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; // 최종 클립 공간 위치
    float2 Tex : TEXCOORD0; // 텍스처 좌표 (보간될 값)
};



VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT Output;

    Output.Pos = float4(Input.Pos, 1.0f); // 입력 위치를 직접 클립 공간 위치로 사용 (풀스크린 쿼드용)
    Output.Tex = Input.Tex; // 텍스처 좌표를 픽셀 셰이더로 전달
    
    return Output;
}
