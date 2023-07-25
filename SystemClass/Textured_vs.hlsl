
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 BoneWeights : BLENDWEIGHT;
    uint4 BoneIds : BLENDINDICES;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TexturedVertexShader(VertexInputType input)
{
    PixelInputType output;

    // ������ ��� ����� ���� ������ ��ġ ���͸� 4������ �ٲߴϴ�.
    input.position.w = 1.0f;

    // ����, �� �� ���� ��Ŀ� ���� ������ ��ġ�� ����մϴ�.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����մϴ�.
    output.tex = input.tex;

    // ���� ��Ŀ� ���ؼ��� ���� ���͸� ����� ���� ����ȭ�մϴ�.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    // ���� ��Ŀ� ���ؼ��� ���� ���͸� ����� ���� ����ȭ�մϴ�.
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    // ���� ��Ŀ� ���ؼ��� �� ��ȿ ���͸� ����� ���� ����ȭ�մϴ�.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);


    // ������ ���� ��ġ�� ����մϴ�.
    float4 worldPosition = mul(input.position, worldMatrix);

    // ī�޶��� ��ġ�� ������ ���� ��ġ�� �������� ���� ������ �����մϴ�.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    // �� ���� ���͸� ǥ��ȭ�մϴ�.
    output.viewDirection = normalize(output.viewDirection);


    return output;
}


