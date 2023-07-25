#include "GraphicComponent.h"
#include "buffer.h"
#include "Object.h"
#include "CameraClass.h"
#include "D3DClass.h"
ID3D11Buffer* GraphicComponent:: m_MatrixBuffer = nullptr;

bool GraphicComponent::Update(ID3D11DeviceContext* devcon, Object* obj)
{
	if (!UpdateWorldMatrix(devcon, obj))
		return false;
	return true;
}

bool GraphicComponent::Initialize(ID3D11Device* dev)
{
	if (!m_MatrixBuffer)
	{
		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.ByteWidth = sizeof(MatrixBufferType);
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;

		HRESULT result = dev->CreateBuffer(&BufferDesc, NULL, &m_MatrixBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}
	return true;
}

bool GraphicComponent::UpdateWorldMatrix(ID3D11DeviceContext* devcon, Object* obj)
{
	XMFLOAT3 rotation = obj->GetRotation();
	XMFLOAT3 scale = obj->GetScale();
	XMFLOAT3 position = obj->GetPosition();
	XMFLOAT3 quat = obj->GetQuaternion();
	XMVECTOR quaternion = XMLoadFloat3(&quat);
	XMFLOAT3 velocity = obj->GetVelocity();

	obj->SetPosition(XMFLOAT3(position.x + velocity.x, position.y + velocity.y, position.z + velocity.z));

	XMMATRIX T = XMMatrixTranslation(position.x + velocity.x, position.y + velocity.y, position.z + velocity.z);
	XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x * 0.0174532925f, rotation.y * 0.0174532925f, rotation.z * 0.0174532925f);
	XMMATRIX Q = XMMatrixRotationQuaternion(quaternion);
	XMMATRIX worldMatrix = S * R * Q * T;
	worldMatrix = XMMatrixTranspose(worldMatrix);
	
	XMMATRIX viewMatrix;
	CameraClass::GetSingleton()->GetViewMatrix(viewMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrix;
	D3DClass::GetSingleton()->GetProjectionMatrix(projectionMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->view = viewMatrix;
	dataPtr->world = worldMatrix;
	dataPtr->projection = projectionMatrix;

	devcon->Unmap(m_MatrixBuffer, 0);

	unsigned int bufferNumber = 0;

	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

	return true;
}


void GraphicComponent::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	// 에러 메시지를 출력창에 표시합니다.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// 에러 메세지를 반환합니다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}
