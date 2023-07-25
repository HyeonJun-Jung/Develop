#include "LightManager.h"
#include "BaseModel.h"
#include "LightClass.h"
#include "buffer.h"
LightManager* LightManager::m_Instance = nullptr;

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

bool LightManager::Initialize(ID3D11Device* dev)
{
	m_DirectionalLight = new LightClass();
	if (!m_DirectionalLight)
		return false;

	m_DirectionalLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_DirectionalLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirectionalLight->SetDirection(1.0f, 0.0f, 1.0f);
	m_DirectionalLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirectionalLight->SetSpecularPower(32.0f);

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.ByteWidth = sizeof(LightBufferType);
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	HRESULT result = dev->CreateBuffer(&BufferDesc, NULL, &m_DirectionalLightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool LightManager::Update(ID3D11DeviceContext* devcon)
{
	RenderLights(devcon);
	return true;
}

bool LightManager::RenderLights(ID3D11DeviceContext* devcon)
{
	RenderDirectionalLight(devcon);
	RenderPointLights(devcon);

	return true;
}

bool LightManager::RenderDirectionalLight(ID3D11DeviceContext* devcon)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_DirectionalLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	LightBufferType* dataPtr = (LightBufferType*)mappedResource.pData;
	dataPtr->ambientColor = m_DirectionalLight->GetAmbientColor();
	dataPtr->diffuseColor = m_DirectionalLight->GetDiffuseColor();
	dataPtr->lightDirection = m_DirectionalLight->GetDirection();
	dataPtr->specularPower = m_DirectionalLight->GetSpecularPower();
	dataPtr->specularColor = m_DirectionalLight->GetSpecularColor();


	// 상수 버퍼의 잠금을 풉니다.
	devcon->Unmap(m_DirectionalLightBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	devcon->PSSetConstantBuffers(bufferNumber, 1, &m_DirectionalLightBuffer);

	return true;
}

bool LightManager::RenderPointLights(ID3D11DeviceContext* devcon)
{
	return true;
}

void LightManager::SetDirectionalLightModel(BaseModel* model)
{
	m_DirectionalLight->SetModel(model);
}

void LightManager::AddPointLight(LightClass* pointLight)
{
	m_PointLightList.push_back(pointLight);
}
