#include "AnimationComponent.h"
#include "buffer.h"
#include "SkinnedObject.h"

ID3D11Buffer* AnimationComponent::m_BoneMatrixBuffer = nullptr;
XMMATRIX* AnimationComponent::m_BoneMatrixs = new XMMATRIX[MAX_BONE];	

AnimationComponent::AnimationComponent()
{
}

bool AnimationComponent::Initialize(ID3D11Device* dev)
{
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.ByteWidth = sizeof(TransformBufferType);
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	HRESULT result = dev->CreateBuffer(&BufferDesc, NULL, &m_BoneMatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

bool AnimationComponent::Update(ID3D11DeviceContext* devcon, SkinnedObject* obj, float fDeltaTime)
{
	if (!UpdateBoneMatrix(devcon, obj, fDeltaTime))
		return false;

	return true;
}

bool AnimationComponent::UpdateBoneMatrix(ID3D11DeviceContext* devcon, SkinnedObject* obj, float fDeltaTime)
{
	SkinnedModel* skinnedModel = obj->GetSkinnedModel();


	if (m_CurAni != -1)
	{
		if (m_AnimationChanged)
		{
			m_CurFrameTime = skinnedModel->GetCurFrameTime(m_CurAni, m_PlayTime);
			m_AnimationChanged = false;
		}

		// 애니메이션이 바뀌었을 시
		if (m_NextAni != -1)
		{
			Animation* animation = skinnedModel->GetAnimation(m_CurAni);

			m_LerpTime += fDeltaTime;

			if (m_LerpTime >= 0.2)
			{
				m_PlayTime = m_LerpTime - 0.2;
				m_LerpTime = 0;
				m_CurAni = m_NextAni;
				m_NextAni = -1;
				skinnedModel->UpdateBoneFinalMatrix(m_CurAni, m_PlayTime, m_BoneMatrixs);
			}
			else
			{
				skinnedModel->ChangeAnimation(m_CurAni, m_NextAni, m_PlayTime, m_LerpTime, m_BoneMatrixs);
			}

			/*
			//if (m_PlayTime + fDeltaTime * animation->TicksPerSecond * 0.1 >= skinnedModel->GetNextFrameTime(m_CurAni, m_PlayTime))
			if (m_PlayTime + fDeltaTime * animation->TicksPerSecond * 0.1 >= m_CurFrameTime + 1)
			{
				m_PlayTime -= skinnedModel->GetNextFrameTime(m_CurAni, m_PlayTime);
				m_PlayTime += fDeltaTime * animation->TicksPerSecond * 0.1;
				m_CurAni = m_NextAni;
				m_NextAni = -1;
				skinnedModel->UpdateBoneFinalMatrix(m_CurAni, m_PlayTime, m_BoneMatrixs);
			}
			else
			{
				m_PlayTime = fmod(m_PlayTime + fDeltaTime * animation->TicksPerSecond * 0.1, animation->Duration);

				// 자연스러운 애니메이션 재생을 위한 BoneMatrix Update
				skinnedModel->ChangeAnimation(m_CurAni, m_NextAni, m_PlayTime, m_BoneMatrixs);
			}
			*/
		}
		else if (m_NextFrame != -1)
		{

		}
		else
		{ 
			Animation* animation = skinnedModel->GetAnimation(m_CurAni);
		
			m_PlayTime = fmod(m_PlayTime + fDeltaTime * animation->TicksPerSecond, animation->Duration);

			// 애니메이션 재생을 위한 BoneMatrix Update
			skinnedModel->UpdateBoneFinalMatrix(m_CurAni, m_PlayTime, m_BoneMatrixs);
		}
	}
	else
	{
		int BoneNum = skinnedModel->GetBoneNum();
		for (int i = 0; i < BoneNum; i++)
		{
			m_BoneMatrixs[i] = XMMatrixIdentity();
		}
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_BoneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	TransformBufferType* dataPtr = (TransformBufferType*)mappedResource.pData;

	for (int i = 0; i < skinnedModel->GetBoneNum(); i++)
		dataPtr->TransformMatrix[i] = XMMatrixTranspose(m_BoneMatrixs[i]);

	devcon->Unmap(m_BoneMatrixBuffer, 0);

	unsigned int bufferNumber = 2;

	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_BoneMatrixBuffer);

	return true;
}