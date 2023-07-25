Texture2D shaderTextures[5];
SamplerState SampleType;
cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float  specularPower;
    float4 specularColor;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
};

float4 TexturedPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    float4 diffColor;
    float4 glossColor;
    float4 emmisiveColor;
    float4 glossiness;

    float4 specularIntensity;
    float3 reflection;
    float4 specular;

    // emmisive map 에서 발광 픽셀을 샘플링합니다.
    emmisiveColor = shaderTextures[4].Sample(SampleType, input.tex);

    if ((emmisiveColor.r == 0.0) && (emmisiveColor.g == 0.0) && (emmisiveColor.b == 0.0))
    {
        // diffuse map에서 확산광 픽셀을 샘플링합니다.
        diffColor = shaderTextures[0].Sample(SampleType, input.tex);

        // 범프맵에서 픽셀을 샘플링합니다.
        bumpMap = shaderTextures[2].Sample(SampleType, input.tex);

        // 법선맵 RGB = 법선벡터 XYZ * 0.5 + 0.5f
        // 법선 벡터의 값의 범위를 (0, +1) 에서 (-1, +1) 까지 넓힙니다.
        bumpMap = (bumpMap * 2.0f) - 1.0f;

        // 범프맵의 데이터에서 (하이폴리곤의) 법선을 계산합니다.
        bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;

        // 계산된 범프 법선을 정규화합니다.
        bumpNormal = normalize(bumpNormal);

        // 계산을 위해 빛의 방향을 반전시킵니다.
        lightDir = -lightDirection;

        // 법프 법선의 값에 기반해 픽셀의 빛의 양을 계산합니다.
        lightIntensity = saturate(dot(bumpNormal, lightDir));

        // 확산 색과 빛의 강도에 따라 최종 확산 색을 결정합니다.
         color = saturate(diffColor * lightIntensity);



         if (lightIntensity > 0.0f)
        {
             // specular map으로부터 픽셀을 샘플링합니다.
             specularIntensity = shaderTextures[1].Sample(SampleType, input.tex);


             // glossiness map에서 광택 픽셀을 샘플링합니다.
             glossiness = shaderTextures[3].Sample(SampleType, input.tex);

             // glossiness 픽셀의 a를 뽑습니다.
             float gloss = glossiness.a;


             // 빛의 강도, 법선 벡터 및 빛의 방향으로 부터 반사 벡터를 계산합니다.
             reflection = normalize(2 * lightIntensity * bumpNormal - lightDir);

             // 반사 벡터, 시선 방향 및 반사 출력을 기준으로 반사 조명의 양을 결정합니다.
             specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

             // specular map을 이용하여 픽셀의 반사광의 강도를 결정합니다.
             specular = specular * specularIntensity * gloss;


             // 출력 색상의 마지막에 반사 컴포넌트를 추가합니다.
             color = saturate(color + specular);
         }
     }
     else
     {
         color = emmisiveColor;
     }

     return color;

}