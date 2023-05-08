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
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǳ�ϴ�.
	devcon->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned int bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

bool ShaderManager::SetWorldMatrix(XMMATRIX worldMatrix)
{
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
	worldMatrix = XMMatrixTranspose(worldMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->world = worldMatrix;

	// ��� ������ ����� Ǳ�ϴ�.
	devcon->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned int bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

bool ShaderManager::SetViewMatrix(XMMATRIX ViewMatrix)
{
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
	ViewMatrix = XMMatrixTranspose(ViewMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->view = ViewMatrix;

	// ��� ������ ����� Ǳ�ϴ�.
	devcon->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned int bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

bool ShaderManager::SetProjectionMatrix(XMMATRIX projectionMatrix)
{
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǳ�ϴ�.
	devcon->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned int bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

}

bool ShaderManager::SetGradientBuffer(XMFLOAT4 ApexColor, XMFLOAT4 CenterColor)
{	
	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_GradientBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	GradientBufferType* dataPtr = (GradientBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->apexColor = ApexColor;
	dataPtr->centerColor = CenterColor;

	// ��� ������ ����� Ǳ�ϴ�.
	devcon->Unmap(m_GradientBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned int bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
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
