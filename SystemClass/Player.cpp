#include "Player.h"
#include "InputClass.h"
#include "PlayerState.h"
#include "..\stdafx.h"
#include "PWalkingState.h"
#include "SkinnedModel.h"
#include "ControllerPhysicsC.h"
#include "PlayerHitCallback.h"

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Initialize(string fileName, SkinnedGraphicC* graphicC, AnimationComponent* animationC)
{
	if (!SkinnedObject::Initialize(fileName, graphicC, animationC))
		return false;
	
	
	if (!InitializePhysics())
		return false;

	m_State = new PWalkingState();
	m_State->Enter(this);

	return true;
}

bool Player::UpdateInput(InputClass* input, float fDeltaTime)
{
	PlayerState* state = m_State->UpdateInput(input, this, fDeltaTime);

	if (state)
	{
		delete m_State;
		m_State = state;
		m_State->Enter(this);
	}

	if(m_PhysicsC)
		m_PhysicsC->UpdatePhysicsVelocity(this, fDeltaTime);

	return true;
}

bool Player::Render(ID3D11DeviceContext* devcon)
{
	SkinnedObject::Render(devcon);

	return true;
}



bool Player::InitializePhysics()
{
	XMMATRIX& xmMatrix = GetWorldMatrix();

	XMFLOAT3 max = *m_SkinnedModel->GetMaxPos(); XMFLOAT3 min = *m_SkinnedModel->GetMinPos();
	float halfy = max.y - min.y;
	PxTransform transform = PxTransform(m_Position.x, m_Position.y , m_Position.z);
	PxMaterial* material = PhysicsComponent::GetPhysics()->createMaterial(0.5, 0.5f, 0.5);
	PxBoxGeometry box = PxBoxGeometry((max.z - min.z), (max.y - min.y) / 2, (max.z - min.z) / 2);
	char name[] = "Player";

	m_PhysicsC = new ControllerPhysicsC();
	if (!m_PhysicsC)
		return false;
	if (!m_PhysicsC->Initialize(this, new PlayerHitCallback(this, m_PhysicsC), name, transform, box, material, 1.f, 10.f))
		return false;

	return true;
}