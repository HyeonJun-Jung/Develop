#pragma once
#include "..\stdafx.h"
#include "Singleton.h"
#include "Shader.h"


class ShaderManager : public Singleton<ShaderManager>
{
public:
	ShaderManager();
	~ShaderManager();

public:
	bool Initialize();

	void InsertShader(Resource_Type rt, Shader* shader);
	Shader* GetShader(Resource_Type rt);

	bool SetMatrixBuffer(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	bool SetWorldMatrix(XMMATRIX worldMatrix);
	bool SetViewMatrix(XMMATRIX ViewMatrix);
	bool SetProjectionMatrix(XMMATRIX projectionMatrix);

	bool SetGradientBuffer(XMFLOAT4 ApexColor, XMFLOAT4 CenterColor);

	bool SetTexture(ID3D11DeviceContext* dev, int startNum, ID3D11ShaderResourceView* texture);
	bool SetTexture(ID3D11DeviceContext* dev, int startNum, int n, ID3D11ShaderResourceView* textures[]);
	bool SetTexture(ID3D11DeviceContext* dev, int startNum, vector<ID3D11ShaderResourceView*> textures);


private:
	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;
	map<Resource_Type, Shader*> ShaderMap;

	ID3D11Buffer* m_matrixBuffer = nullptr;
	ID3D11Buffer* m_cameraBuffer = nullptr;
	ID3D11Buffer* m_lightBuffer = nullptr;

	ID3D11Buffer* m_ReflectionBuffer = nullptr;
	ID3D11Buffer* m_ReflectionLightBuffer = nullptr;
	ID3D11Buffer* m_TerrainLightBuffer = nullptr;

	ID3D11Buffer* m_ClipPlaneBuffer = nullptr;
	ID3D11Buffer* m_GradientBuffer = nullptr;
	ID3D11Buffer* m_WaterBuffer = nullptr;
};

