#include "SkinnedObject.h"
#include "ModelManager.h"
#include "SkinnedModel.h"
#include "SkinnedGraphicC.h"
#include "ControllerPhysicsC.h"
#include "AnimationComponent.h"
#include "..\stdafx.h"

SkinnedObject::SkinnedObject()
{
}

SkinnedObject::~SkinnedObject()
{
}

bool SkinnedObject::Initialize(string fileName, SkinnedGraphicC* graphicC, AnimationComponent* animationC)
{
	m_SkinnedModel = ModelManager::GetSingleton()->GetModel(fileName);
	if (!m_SkinnedModel)
		return false;

	m_AnimationC = animationC;
	m_GraphicC = graphicC;

	//float rotation = 0.1745 * 90;

	XMVECTOR Translation = XMVectorSet(0, 0, 300, 1);
	XMVECTOR Z = XMVectorSet(0, 0, 0, 1);
	m_WorldMatrix = XMMatrixTranslation(0, 0, 300);

	m_AnimationC->SetCurAnimation(0);

	return true;
}

bool SkinnedObject::UpdateInput(InputClass* input)
{
	return true;
}

bool SkinnedObject::UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime)
{
	if (!m_AnimationC->Update(devcon, this, fDeltaTime))
		return false;

	return true;
}

bool SkinnedObject::UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime)
{
	if (!m_PhysicsC->Update(this))
		return false;

	return true;
}

bool SkinnedObject::Render(ID3D11DeviceContext* devcon)
{
	if (!m_GraphicC->Update(devcon, this))
		return false;

	return true;
}

bool SkinnedObject::IsOnGround()
{
	return m_PhysicsC->IsOnGround();
}

void SkinnedObject::SetNextAnimation(int animationIndex)
{
	m_AnimationC->SetNextAnimation(animationIndex);
}

void SkinnedObject::SetAnimationFrame(int frame)
{
	// m_SkinnedModel->GetAnimation(m_AnimationIndex)->aninodeList[]
}
