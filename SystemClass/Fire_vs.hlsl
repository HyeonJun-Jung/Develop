cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix inverseViewMatrix;
    matrix projectionMatrix;
};

cbuffer SparkBuffer : register(b1)
{
    float time;
    float duration;
    //
    float2 startSize;
    float3 startDirection;
    //
    float sizeOverLifeTime;
    float3 rotOverLifeTime;
}

struct VertexInput
{
    float4 pos : Position;
    float2 size : Size;
    float3 direction : Direction;
    //
    float startLifeTime : LifeTime;
    float speed : Speed;
    float4 particleColor : Color;
};

struct VertexOutput
{
    float3 pos : Position;
    float2 size : Size;
    float time : Time;
    //
    float4 particleColor : Color;
};

VertexOutput FireVertexShader(VertexInput input)
{
    VertexOutput output;
    // 0 ~ 1의 비율로 전달
    output.time = time / input.startLifeTime;
    //output.time = time / duration;
    //
    input.direction += startDirection * time;
    //input.direction *= time;
    input.pos = mul(input.pos, worldMatrix);
    output.pos = input.pos.xyz + (input.direction * time) * input.speed;
    output.size = input.size;
    output.particleColor = input.particleColor;
    //
    return output;
}
