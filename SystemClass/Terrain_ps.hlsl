
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float	specularPower;
	float4 specularColor;
};

cbuffer BrushBuffer : register(b1)
{
	int type;
	float3 location;
	float range;
	float3 color;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPos : Position;
};


float3 BrushColor(float3 pos)
{
	if (type == 0)
	{
		float x = pos.x - location.x;
		float z = pos.z - location.z;
		//
		float distance = sqrt(x * x + z * z);
		//
		if (distance <= range)
			return color;
	}
	else if (type == 1)
	{
		float x = pos.x - location.x;
		float z = pos.z - location.z;
		//
		float distX = abs(x);
		float distZ = abs(z);
		//
		if (distX <= range && distZ <= range)
			return color;
	}
	//
	return float3(0, 0, 0);
}

float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;


	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// ��� �ȼ��� ���� �⺻ ��� ������ �ֺ� ���� ������ �����մϴ�.
	color = ambientColor;

	// ����� ���� �� ������ ������ŵ�ϴ�.
	lightDir = -lightDirection;

	// �� �ȼ��� ���� ���� ����մϴ�.
	lightIntensity = saturate(dot(input.normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		// Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.
		color += (diffuseColor * lightIntensity);
	}

	// ���� ���� ������ ä��ϴ�.
	color = saturate(color);

	float4 brushColor = (BrushColor(input.worldPos), 0);

	// �ؽ�ó �ȼ��� ���� ���� ���� ���Ͽ� ����� ����ϴ�.
	if (brushColor.y != 0 || brushColor.x != 0 || brushColor.z != 0)
		return brushColor;

	color = color * textureColor + brushColor;

	

	return color;
}