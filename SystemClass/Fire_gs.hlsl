cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix inverseViewMatrix;
	matrix projectionMatrix;
};

struct VertexOutput
{
	float3 pos : Position;
	float2 size : Size;
	float time : Time;
	//
	float4 particleColor : Color;
};

struct PixelInput
{
	float4 pos : SV_Position;
	float2 uv : UV;
	float time : Time;
	float4 particleColor : Color;
};

static const float2 TEXCOORD[4] =
{
	float2(0.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 1.0f),
	float2(1.0f, 0.0f)
};

[maxvertexcount(4)]
void FireGeometryShader(point VertexOutput input[1], inout TriangleStream<PixelInput> output)
{
	float3 camPos = inverseViewMatrix._41_42_43;
	//
	float3 up = inverseViewMatrix._21_22_23;
	//
	float3 forward = camPos - input[0].pos;
	forward = normalize(forward);
	//
	float3 right = normalize(cross(up, forward));
	//
	float halfWidth = input[0].size.x * 0.5f;
	float halfHeight = input[0].size.y * 0.5f;
	//
	float4 vertices[4];
	vertices[0] = float4(input[0].pos + halfWidth * right - halfHeight * up, 1.0f);
	vertices[1] = float4(input[0].pos + halfWidth * right + halfHeight * up, 1.0f);
	vertices[2] = float4(input[0].pos - halfWidth * right - halfHeight * up, 1.0f);
	vertices[3] = float4(input[0].pos - halfWidth * right + halfHeight * up, 1.0f);
	//
	PixelInput pixelInput;
	pixelInput.time = input[0].time;
	//
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		pixelInput.pos = mul(vertices[i], viewMatrix);
		pixelInput.pos = mul(pixelInput.pos, projectionMatrix);
		pixelInput.uv = TEXCOORD[i];
		pixelInput.particleColor = input[0].particleColor;
		// pushback
		output.Append(pixelInput);
	}
}