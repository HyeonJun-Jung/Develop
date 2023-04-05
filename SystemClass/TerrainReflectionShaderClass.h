#pragma once
#pragma once
#include "..\stdafx.h"
#include "Shader.h"
class TerrainReflectionShaderClass : public Shader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct ClipPlaneBufferType
	{
		XMFLOAT4 clipPlane;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

public:
	TerrainReflectionShaderClass();
	TerrainReflectionShaderClass(const TerrainReflectionShaderClass&);
	~TerrainReflectionShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexcount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 LightDirection, XMFLOAT4 ClipPlane);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4);




private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11Buffer* m_matrixBuffer = nullptr;
	ID3D11Buffer* m_ClipPlaneBuffer = nullptr;
	ID3D11Buffer* m_lightBuffer = nullptr;

};

