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

	XMMATRIX viewMatrix; CameraClass::GetSingleton()->GetViewMatrix(viewMatrix);
	XMMATRIX projectionMatrix; D3DClass::GetSingleton()->GetProjectionMatrix(projectionMatrix);

	ShaderManager::GetSingleton()->SetMatrixBuffer(m_WorldMatrix, viewMatrix, projectionMatrix);


	return true;
}

bool BaseModel::Update(float fDeltaTime)
{
	
    return true;
}

void BaseModel::SetPosition(float PositionX, float PositionY, float PositionZ)
{
	m_TranslationMatrix = XMMatrixTranslation(PositionX, PositionY, PositionZ);
}

void BaseModel::SetScale(float ScaleX, float ScaleY, float ScaleZ)
{
	m_ScaleMatrix = XMMatrixScaling(ScaleX, ScaleY, ScaleZ);
	m_WorldMatrix = m_ScaleMatrix * m_RotationMatrix * m_TranslationMatrix;
}

void BaseModel::SetRotation(float RotationX, float RotationY, float RotationZ)
{
	float pitch = RotationX * 0.0174532925f;
	float yaw = RotationY * 0.0174532925f;
	float roll = RotationZ * 0.0174532925f;
	m_RotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

void BaseModel::AddPosition(float PositionX, float PositionY, float PositionZ)
{
	m_TranslationMatrix = m_TranslationMatrix + XMMatrixTranslation(PositionX, PositionY, PositionZ);
}

void BaseModel::AddScale(float ScaleX, float ScaleY, float ScaleZ)
{
	m_ScaleMatrix = m_ScaleMatrix + XMMatrixScaling(ScaleX, ScaleY, ScaleZ);
}

void BaseModel::AddRotation(float RotationX, float RotationY, float RotationZ)
{
	float pitch = RotationX * 0.0174532925f;
	float yaw = RotationY * 0.0174532925f;
	float roll = RotationZ * 0.0174532925f;
	m_RotationMatrix = m_RotationMatrix + XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

XMMATRIX BaseModel::GetWorldMatrix()
{
	return m_WorldMatrix;
}

bool BaseModel::CreateObb()
{
	return true;
}

bool BaseModel::CreateSphere()
{
	return true;
}
