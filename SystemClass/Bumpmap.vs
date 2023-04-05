////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmap.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

cbuffer TransformBuffer
{
    matrix TransformMatrix[100];
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
PixelInputType BumpMapVertexShader(VertexInputType input)
{
    PixelInputType output;

    matrix bonetransform = {{ 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }};
            
    
    if(input.BoneWeights.x != 0)
    {
       float finalweight = 1 - (input.BoneWeights.z + input.BoneWeights.x +  input.BoneWeights.y );
        
        bonetransform = TransformMatrix[input.BoneIds.x] * input.BoneWeights.x;  
        bonetransform += TransformMatrix[input.BoneIds.y] * input.BoneWeights.y;
        bonetransform += TransformMatrix[input.BoneIds.z] * input.BoneWeights.z;
        bonetransform += TransformMatrix[input.BoneIds.w] * finalweight;        
   }
    
   
   
    // 적절한 행렬 계산을 위해 정점의 위치 벡터를 4단위로 바꿉니다.

    input.position.w = 1.0f;

    // 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 계산합니다.
    output.position = mul(input.position, bonetransform);
    output.position = mul(output.position, worldMatrix);  
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 픽셀 쉐이더의 텍스처 좌표를 저장합니다.
    output.tex = input.tex;
    
    // 월드 행렬에 대해서만 법선 벡터를 계산한 다음 정규화합니다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    // 월드 행렬에 대해서만 접선 벡터를 계산한 다음 정규화합니다.
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    // 월드 행렬에 대해서만 비 유효 벡터를 계산한 다음 정규화합니다.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);

    
    // 세계의 정점 위치를 계산합니다.
    float4 worldPosition = mul(input.position, bonetransform);
    worldPosition = mul(worldPosition, worldMatrix);

    // 카메라의 위치와 세계의 정점 위치를 기준으로 보기 방향을 결정합니다.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
    
    // 뷰 방향 벡터를 표준화합니다.
    output.viewDirection = normalize(output.viewDirection);
    

    return output;
}


