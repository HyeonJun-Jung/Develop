#pragma once

#include "..\DxDefine.h"

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct CameraBufferType
{
	XMFLOAT3 cameraPosition;
	float padding;
};

struct ReflectionBufferType
{
	XMMATRIX reflection;
};

/// <summary>
/// Lights
/// </summary>

struct LightBufferType
{
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 lightDirection;
	float	 specularPower;
	XMFLOAT4 specularColor;
};

struct ReflectionLightBufferType
{
	XMFLOAT4 lightDiffuseColor;
	XMFLOAT3 lightDirection;
	float colorTextureBrightness;
};

struct TerrainLightBufferType
{
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 lightDirection;
	float padding;
};

/// <summary>
/// Terrain
/// </summary>


struct ClipPlaneBufferType
{
	XMFLOAT4 clipPlane;
};

/// <summary>
/// Skydome
/// </summary>

struct GradientBufferType
{
	XMFLOAT4 apexColor;
	XMFLOAT4 centerColor;
};


/// <summary>
/// Water
/// </summary>

struct WaterBufferType
{
	float waterTranslation;
	float reflectRefractScale;
	XMFLOAT2 padding;
};