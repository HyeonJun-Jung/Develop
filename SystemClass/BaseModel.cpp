#include "BaseModel.h"
#include "ShaderManager.h"
#include "D3DClass.h"
#include "CameraClass.h"

BaseModel::BaseModel()
{
}

BaseModel::~BaseModel()
{
}


bool BaseModel::Render()
{
	ShaderManager::GetSingleton()->GetShader(m_RT)->RenderShader(D3DClass::GetSingleton()->GetDeviceContext());

	XMMATRIX worldMatrix = BaseModel::GetWorldMatrix();
	XMMATRIX viewMatrix; CameraClass::GetSingleton()->GetViewMatrix(viewMatrix);
	XMMATRIX projectionMatrix; D3DClass::GetSingleton()->GetProjectionMatrix(projectionMatrix);
		
	ShaderManager::GetSingleton()->SetMatrixBuffer(worldMatrix, viewMatrix, projectionMatrix);

	return true;
}

bool BaseModel::Update(float fDeltaTime)
{
    return true;
}

void BaseModel::SetPosition(float PositionX, float PositionY, float PositionZ)
{
	m_WMType.Transx = PositionX;
	m_WMType.Transy = PositionY;
	m_WMType.Transz = PositionZ;
}

void BaseModel::SetScale(float ScaleX, float ScaleY, float ScaleZ)
{
	m_WMType.ScaleX = ScaleX;
	m_WMType.ScaleY = ScaleY;
	m_WMType.ScaleZ = ScaleZ;
}

void BaseModel::SetRotation(float RotationX, float RotationY, float RotationZ)
{
	m_WMType.RotationX = RotationX;
	m_WMType.RotationY = RotationY;
	m_WMType.RotationZ = RotationZ;
}

void BaseModel::AddPosition(float PositionX, float PositionY, float PositionZ)
{
	m_WMType.Transx += PositionX;
	m_WMType.Transy += PositionY;
	m_WMType.Transz += PositionZ;
}

void BaseModel::AddScale(float ScaleX, float ScaleY, float ScaleZ)
{
	m_WMType.ScaleX += ScaleX;
	m_WMType.ScaleY += ScaleY;
	m_WMType.ScaleZ += ScaleZ;
}

void BaseModel::AddRotation(float RotationX, float RotationY, float RotationZ)
{
	m_WMType.RotationX += RotationX;
	m_WMType.RotationY += RotationY;
	m_WMType.RotationZ += RotationZ;
}

XMMATRIX BaseModel::GetWorldMatrix()
{
	float pitch = m_WMType.RotationX * 0.0174532925f;
	float yaw = m_WMType.RotationY * 0.0174532925f;
	float roll = m_WMType.RotationZ * 0.0174532925f;

	XMMATRIX WorldMatrix = XMMatrixScaling(m_WMType.ScaleX, m_WMType.ScaleY, m_WMType.ScaleZ)
		* XMMatrixRotationRollPitchYaw(pitch, yaw, roll)
		* XMMatrixTranslation(m_WMType.Transx, m_WMType.Transy, m_WMType.Transz);
	return WorldMatrix;
}