#pragma once
#include <PxPhysicsAPI.h>
#define PVD_HOST "127.0.0.1"
using namespace physx;
class Object;
class PhysicsComponent
{
public:
    PhysicsComponent();
    virtual ~PhysicsComponent();

public:
    virtual bool Update(Object* obj) = 0;

    static bool InitPhysics();
    static bool UpdatePhysics(float fDeltaTime);
    static PxPhysics* GetPhysics() { return m_Physics; }
    static PxCooking* GetCooking() { return m_Cooking; }
    static PxScene* GetScene() { return m_Scene; }
    static void ReleasePhysics();

private:
    static PxDefaultAllocator*		m_Allocator;
    static PxDefaultErrorCallback*	m_ErrorCallback;

protected:
    static PxPvd* m_Pvd;
    static PxFoundation* m_Foundation;
    static PxPhysics* m_Physics;
    static PxCooking* m_Cooking;
    static PxDefaultCpuDispatcher* m_Dispatcher;
    static PxScene* m_Scene;

};

