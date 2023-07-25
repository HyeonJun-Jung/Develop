#include "PhysicsComponent.h"
 PxPvd* PhysicsComponent::m_Pvd = nullptr;;
 PxFoundation* PhysicsComponent::m_Foundation = nullptr;
 PxPhysics* PhysicsComponent::m_Physics = nullptr;
 PxCooking* PhysicsComponent::m_Cooking = nullptr;
 PxDefaultCpuDispatcher* PhysicsComponent::m_Dispatcher = nullptr;
 PxScene* PhysicsComponent::m_Scene = nullptr;
 PxDefaultAllocator* PhysicsComponent::m_Allocator = new PxDefaultAllocator();
 PxDefaultErrorCallback* PhysicsComponent::m_ErrorCallback = new PxDefaultErrorCallback();

PhysicsComponent::PhysicsComponent()
{
}

PhysicsComponent::~PhysicsComponent()
{
}

bool PhysicsComponent::Update(Object* obj)
{

	return true;
}

bool PhysicsComponent::InitPhysics()
{
	if (!m_Foundation)
	{
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *m_Allocator, *m_ErrorCallback);
		if (!m_Foundation)
			return false;
		m_Pvd = PxCreatePvd(*m_Foundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 100);
		m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true, m_Pvd);
		if (!m_Physics)
			return false;

		m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams(PxTolerancesScale()));
		if (!m_Cooking)
			return false;

		PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = m_Dispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		m_Scene = m_Physics->createScene(sceneDesc);
		
		PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		PxMaterial* material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);

		PxRigidStatic* groundPlane = PxCreatePlane(*m_Physics, PxPlane(0, 1, 0, 0), *material);
		m_Scene->addActor(*groundPlane);
	}
	return true;
}

bool PhysicsComponent::UpdatePhysics(float fDeltaTime)
{
	if (!m_Foundation)
		return false;

	m_Scene->simulate(fDeltaTime);
	if (!m_Scene->fetchResults(true))
		return false;

	return true;
}

void PhysicsComponent::ReleasePhysics()
{
	m_Physics->release();
	m_Foundation->release();
}
