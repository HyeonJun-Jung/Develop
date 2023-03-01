/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ReflectionBuffer
{
    matrix reflectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 reflectionPosition : TEXCOORD1;
};

PixelInputType ReflectionVertexShader(VertexInputType input)
{
    PixelInputType output;
    matrix reflectProjectWorld;
    

    // 정상적인 행렬 연산을 위해 위치 벡터의 마지막 값을 수정합니다.
    input.position.w = 1.0f;

    // 정점의 위치를 월드, 뷰, 프로젝션 행렬을 이용하여 계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 셰이더에 넘길 텍스쳐 좌표를 저장합니다.
    output.tex = input.tex;

    // 반사 투영 월드 행렬을 생성합니다.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    // reflectProjectWorld 행렬에 비추어 입력 위치를 계산합니다.
    output.reflectionPosition = mul(input.position, reflectProjectWorld);

    return output;
}