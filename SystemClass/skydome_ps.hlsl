////////////////////////////////////////////////////////////////////////////////
// Filename: skydome_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float	specularPower;
	float4 specularColor;
};

cbuffer GradientBuffer : register(b1)
{
	float4 apexColor;
	float4 centerColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 domePosition : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 SkyDomePixelShader(PixelInputType input) : SV_TARGET
{
	// �� �ȼ��� �ִ� ��ī�� ������ ��ġ�� �����մϴ�.
	float height = input.domePosition.y;

	// ���� ������ -1.0f���� + 1.0f�̹Ƿ� ��� ���� �����Ͻʽÿ�.
	if (height < 0.0)
	{
		height = 0.0f;
	}

	// ��ī�� ������ �ȼ��� ���̸� �������� ������ �߽��� �����Ͽ� �׶���Ʈ ������ �����մϴ�.
	float4 outputColor = lerp(centerColor, apexColor, height);

	return outputColor;
}