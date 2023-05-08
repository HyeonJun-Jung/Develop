#include "ShaderManager.h"
#include "D3DClass.h"
#include "buffer.h"


ShaderManager* ShaderManager::m_Instance = nullptr;
 
ShaderManager::ShaderManager()
{
	
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::InsertShader(Resource_Type rt, Shader* shader)
{
	ShaderMap.insert(make_pair(rt, shader));
}

Shader* ShaderManager::GetShader(Resource_Type rt)
{
	return ShaderMap[rt];
}

bool ShaderManager::Initialize()
{
	dev = D3DClass::GetSingleton()->GetDevice();
	devcon = D3DClass::GetSingleton()->GetDeviceContext();

	HRESULT result; 

	/// <summary>
	/// MatrixBufferType
	/// 

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.ByteWidth = sizeof(MatrixBufferType);
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/// <summary>
	/// CameraBufferType
	/// </summary>

	BufferDesc.ByteWidth = sizeof(CameraBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}

	///
	///	ReflectionBufferType
	/// 
	
	BufferDesc.ByteWidth = sizeof(ReflectionBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_ReflectionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/// <summary>
	/// LightBufferType
	/// </summary>

	BufferDesc.ByteWidth = sizeof(LightBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	///
	/// ReflectionLightBufferType
	///
	
	BufferDesc.ByteWidth = sizeof(ReflectionLightBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_ReflectionLightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	///
	/// TerrainLightBufferType
	/// 

	BufferDesc.ByteWidth = sizeof(TerrainLightBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_TerrainLightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/// <summary>
	/// ClipPlaneBufferType
	/// </summary>

	BufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_ClipPlaneBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/// <summary>
	/// GradientBufferType
	/// </summary>

	BufferDesc.ByteWidth = sizeof(GradientBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_GradientBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/// <summary>
	/// WaterBufferType
	/// </summary>

	BufferDesc.ByteWidth = sizeof(WaterBufferType);

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_WaterBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


bool ShaderManager::SetMatrixBuffer(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	devcon->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

bool ShaderManager::SetWorldMatrix(XMMATRIX worldMatrix)
{
	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
	worldMatrix = XMMatrixTranspose(worldMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	devcon->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

bool ShaderManager::SetViewMatrix(XMMATRIX ViewMatrix)
{
	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
	ViewMatrix = XMMatrixTranspose(ViewMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->view = ViewMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	devcon->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

bool ShaderManager::SetProjectionMatrix(XMMATRIX projectionMatrix)
{
	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	devcon->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

}

bool ShaderManager::SetGradientBuffer(XMFLOAT4 ApexColor, XMFLOAT4 CenterColor)
{	
	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_GradientBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	GradientBufferType* dataPtr = (GradientBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->apexColor = ApexColor;
	dataPtr->centerColor = CenterColor;

	// 상수 버퍼의 잠금을 풉니다.
	devcon->Unmap(m_GradientBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	devcon->PSSetConstantBuffers(bufferNumber, 1, &m_GradientBuffer);
}

bool ShaderManager::SetTexture(ID3D11DeviceContext* dev, int startNum, ID3D11ShaderResourceView* texture)
{
	dev->PSSetShaderResources(startNum, 1, &texture);
	return true;
}

bool ShaderManager::SetTexture(ID3D11DeviceContext* dev, int startNum, int n, ID3D11ShaderResourceView* textures[])
{
	for (int i = 0; i < n; i++)
	{
		dev->PSSetShaderResources(startNum + i, 1, &textures[i]);
	}
	return true;
}

bool ShaderManager::SetTexture(ID3D11DeviceContext* dev, int startNum, vector<ID3D11ShaderResourceView*> textures)
{
	for (int i = 0; i < textures.size(); i++)
	{
		dev->PSSetShaderResources(startNum + i, 1, &textures[i]);
	}
	return true;
}
