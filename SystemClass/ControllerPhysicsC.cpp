#include "ControllerPhysicsC.h"
#include "..\stdafx.h"
#include "SkinnedObject.h"
#include "SkinnedModel.h"
#include <PxController.h>
#include <PxControllerManager.h>
#include "ControllerCallBack.h"
ControllerPhysicsC::ControllerPhysicsC()
{
}

ControllerPhysicsC::~ControllerPhysicsC()
{
}

bool ControllerPhysicsC::Initialize(SkinnedObject* obj, ControllerCallBack* hitCallBack, char* name,
	const PxTransform& t, const PxGeometry& geometry, PxMaterial* material,
	float density, float mass, const PxVec3& velocity)
{
	SkinnedModel* model = obj->GetSkinnedModel();
	XMFLOAT3 maxPos = *model->GetMaxPos();
	XMFLOAT3 minPos = *model->GetMinPos();

	m_CallBack = hitCallBack;

	PxCapsuleControllerDesc controllerDesc;
	controllerDesc.setToDefault();
	controllerDesc.material = material;
	controllerDesc.radius = 30;
	controllerDesc.height = maxPos.y - minPos.y - 8;
	controllerDesc.density = 10;
	controllerDesc.upDirection = PxVec3(0, 1, 0);
	controllerDesc.stepOffset = 0.5;
	// controllerDesc.slopeLimit = cosf()
	controllerDesc.contactOffset = 0.001;
	controllerDesc.scaleCoeff = 0.999;
	controllerDesc.reportCallback = m_CallBack;
	controllerDesc.climbingMode = PxCapsuleClimbingMode::eEASY;
	
	PxControllerManager* manager = PxCreateControllerManager(*PhysicsComponent::GetScene());
	manager->setOverlapRecoveryModule(true);
	m_Controller = manager->createController(controllerDesc);
	XMFLOAT3 pos = obj->GetPosition();

	PxExtendedVec3 foot = PxExtendedVec3(pos.x, pos.y, pos.z);
	// m_Controller->setFootPosition(foot);
	m_Controller->setPosition(foot);
	m_Controller->getActor()->setName(name);

	/*
	PxShape* shape = nullptr;
	m_Controller->getActor()->getShapes(&shape, 1, 0);
	
	PxFilterData filter;
	filter.setToDefault();
	filter.word0 = 1;
	shape->setQueryFilterData(filter);
	shape->setSimulationFilterData(filter);
	*/

	return true;
}

bool ControllerPhysicsC::UpdatePhysicsVelocity(SkinnedObject* obj, float fDeltaTime)
{
	SkinnedModel* model = obj->GetSkinnedModel();
	XMFLOAT3 velocity = obj->GetVelocity();
	PxVec3 temp = PxVec3(velocity.x, velocity.y, velocity.z);

	// CCT와 CCT 사이의 충돌을 정의하기 위한 구조체
	
	PxControllerFilters filter;
	PxFilterData filterData = PxFilterData();
	filterData.setToDefault();
	filterData.word0 = 0;
	filter.mFilterData = &filterData;
	filter.mFilterCallback = m_CallBack;
	filter.mFilterFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;
	filter.mCCTFilterCallback = NULL;

	PxControllerCollisionFlags collisionFlags = m_Controller->move(temp, 0.01 , fDeltaTime, filter);
	// collisionFlags = m_Controller->move(PxVec3(0, -9.8, 0), 0.01, fDeltaTime, filter);
	
	PxControllerState state; m_Controller->getState(state);

	if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_IsOnGround = true;
	}
	else
	{
		m_IsOnGround = false;
	}


	return true;
}

bool ControllerPhysicsC::Update(Object* obj)
{
	PxExtendedVec3 position = m_Controller->getPosition();
	PxVec3 upVector = m_Controller->getUpDirection();

	//	PxTransform transform = m_Controller->getFootPosition->getGlobalPose();
	XMFLOAT3 trans = XMFLOAT3(position.x, position.y, position.z);
	XMFLOAT3 scale = XMFLOAT3(1, 1, 1);
	//XMFLOAT3 quat = XMFLOAT3(transform.q.x, transform.q.y, transform.q.z);

	obj->SetPosition(trans);
	//obj->SetQuaternion(quat);

	return true;
}
