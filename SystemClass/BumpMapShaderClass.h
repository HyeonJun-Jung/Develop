#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
using namespace DirectX;

class BumpMapShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	class CameraBufferType
	{
	public:
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float	 specularPower;
		XMFLOAT4 specularColor;
	};

public:
	BumpMapShaderClass();
	BumpMapShaderClass(const BumpMapShaderClass&);
	~BumpMapShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	bool Render(ID3D11DeviceContext* deviceContext, XMFLOAT3 cameraPosition, XMFLOAT4 ambientColor,
		XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT4 specularColor, float specularPower);

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView** texture, XMFLOAT3 cameraPosition, XMFLOAT4 ambientColor,
		XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT4 specularColor, float specularPower);

	bool SetMatrixBuffer(ID3D11DeviceContext* deviceContext,  XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**, XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float);
	bool SetShaderParameters(ID3D11DeviceContext*, XMFLOAT3 cameraPosition , XMFLOAT4 ambientColor,
		XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT4 specularColor, float specularPower);


	void RenderShader(ID3D11DeviceContext*, int);
	void RenderShader(ID3D11DeviceContext* devcon);

private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	ID3D11Buffer* m_matrixBuffer = nullptr;
	ID3D11Buffer* m_cameraBuffer = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11Buffer* m_lightBuffer = nullptr;
};