Texture2D diffuseMap : register(t0);
SamplerState SampleType;

cbuffer ColorBuffer : register(b0)
{
    float4 totalColor;
    float4 colorOverLifeTime;
}

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
    float time : Time;
    float4 particleColor : Color;
};

//
float4 FirePixelShader(PixelInput input) : SV_Target
{
    float4 albedo = diffuseMap.Sample(SampleType, input.uv);
    //
    float4 color = lerp(input.particleColor, colorOverLifeTime, input.time);
    //

    return albedo;
}