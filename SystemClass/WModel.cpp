#include "WModel.h"
#include "ModelManager.h"
#include "BumpMapShaderClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "SkinnedModel.h"
#include "Obb.h"

WModel::WModel()
{
}

WModel::~WModel()
{
}

bool WModel::LoadWModel(string ModelDir)
{
	m_Skinnedmodel = ModelManager::GetSingleton()->GetModel(ModelDir);
	if (!m_Skinnedmodel)
		return false;

	// 버텍스 쉐이더에있는 변환 행렬 상수 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC TransformBufferDesc;
	TransformBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	TransformBufferDesc.ByteWidth = sizeof(TransformBufferType);
	TransformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TransformBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TransformBufferDesc.MiscFlags = 0;
	TransformBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있도록 변환 행렬 상수 버퍼 포인터를 만듭니다.
	if (FAILED(D3DClass::GetSingleton()->GetDevice()->CreateBuffer(&TransformBufferDesc, NULL, &m_TransformBuffer)))
	{
		return false;
	}

	return true;
}

bool WModel::Render()
{	
	m_Skinnedmodel->Render(D3DClass::GetSingleton()->GetDeviceContext());

	return true;
}

bool WModel::Update(float fDeltaTime)
{
	/// <summary>
	///	상수 버퍼 세팅
	/// </summary>

	float pitch = m_WMType.RotationX * 0.0174532925f;
	float yaw = m_WMType.RotationY * 0.0174532925f;
	float roll = m_WMType.RotationZ * 0.0174532925f;

	XMMATRIX Translation = XMMatrixTranslation(m_WMType.Transx, m_WMType.Transy, m_WMType.Transz);
	XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	XMMATRIX Scale = XMMatrixScaling(m_WMType.ScaleX, m_WMType.ScaleY, m_WMType.ScaleZ);

	XMMATRIX worldMatrix = Scale * Rotation * Translation;
	XMMATRIX viewMatrix; CameraClass::GetSingleton()->GetViewMatrix(viewMatrix);
	XMMATRIX projectionMatrix; D3DClass::GetSingleton()->GetProjectionMatrix(projectionMatrix);



	if (PlayAni != -1)
	{
		vector<Animation>* AniList = m_Skinnedmodel->GetAnimationList();
		Animation* animation = &(*AniList)[PlayAni];
		PlayTime = fmod(PlayTime + fDeltaTime * animation->TicksPerSecond, animation->Duration);
		m_Skinnedmodel->UpdateBoneFinalMatrix(PlayAni, PlayTime, BoneFinalTransform);

		/*
		worldMatrix = m_Skinnedmodel->GetRootNodeTransform(PlayAni, PlayTime, m_Skinnedmodel->GetRootNode());

		int BoneNum = m_Skinnedmodel->GetBoneNum();
		for (int i = 0; i < BoneNum; i++)
		{
			BoneFinalTransform[i] = XMMatrixIdentity();

		}
		*/
	}
	else
	{
		int BoneNum = m_Skinnedmodel->GetBoneNum();
		for (int i = 0; i < BoneNum; i++)
		{
			BoneFinalTransform[i] = XMMatrixIdentity();
		}
	}

	if (!m_Shader->SetMatrixBuffer(D3DClass::GetSingleton()->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix))
		return false;

	if (!SetBoneTransform())
		return false;

}

bool WModel::SetShader(BumpMapShaderClass* Shader)
{
	m_Shader = Shader;
	return true;
}

void WModel::PlayAnimation(int AnimationIndex)
{
	PlayAni = AnimationIndex;
}

void WModel::StopAnimation()
{
	PlayAni = -1;
}

void WModel::UpdateAnimation(float Deltatime)
{
}

bool WModel::SetBoneTransform()
{
	int BoneNum = m_Skinnedmodel->GetBoneNum();

	// 쓸 수 있도록 카메라 상수 버퍼를 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (FAILED(D3DClass::GetSingleton()->GetDeviceContext()->Map(m_TransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	TransformBufferType* dataPtr3 = (TransformBufferType*)mappedResource.pData;

	// 최종 변환 행렬을 상수 버퍼에 복사합니다.
	for (int i = 0; i < BoneNum; i++)
	{
		dataPtr3->TransformMatrix[i] = BoneFinalTransform[i];
	}

	// 카메라 상수 버퍼를 잠금 해제합니다.
	D3DClass::GetSingleton()->GetDeviceContext()->Unmap(m_TransformBuffer, 0);

	// 버텍스 쉐이더에서 카메라 상수 버퍼의 위치를 ​​설정합니다.
	int bufferNumber = 2;

	// 이제 업데이트 된 값으로 버텍스 쉐이더에서 카메라 상수 버퍼를 설정합니다.
	D3DClass::GetSingleton()->GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &m_TransformBuffer);

	return true;
}

void WModel::ReleaseModel()
{
	m_Skinnedmodel = nullptr;
}

void WModel::SetPosition(float PositionX, float PositionY, float PositionZ)
{
	m_WMType.Transx = PositionX;
	m_WMType.Transy = PositionY;
	m_WMType.Transz = PositionZ;
}

void WModel::SetScale(float ScaleX, float ScaleY, float ScaleZ)
{
	m_WMType.ScaleX = ScaleX;
	m_WMType.ScaleY = ScaleY;
	m_WMType.ScaleZ = ScaleZ;
}

void WModel::SetRotation(float RotationX, float RotationY, float RotationZ)
{
	m_WMType.RotationX = RotationX;
	m_WMType.RotationY = RotationY;
	m_WMType.RotationZ = RotationZ;
}