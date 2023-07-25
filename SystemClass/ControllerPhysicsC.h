#pragma once
#include "PhysicsComponent.h"
class ControllerCallBack;
class SkinnedObject;

class ControllerPhysicsC :
    public PhysicsComponent
{
public:
    ControllerPhysicsC();
    virtual ~ControllerPhysicsC();

public:
    bool Initialize(SkinnedObject* obj, ControllerCallBack* hitCallBack, char* name,
        const PxTransform& t, const PxGeometry& geometry, PxMaterial* material,
        float density, float mass, const PxVec3& velocity = PxVec3(0));
    bool UpdatePhysicsVelocity(SkinnedObject* obj, float fDeltaTime);
    bool Update(Object* obj);

    bool IsOnGround() { return m_IsOnGround; }

private:
    PxController* m_Controller = nullptr;
    ControllerCallBack* m_CallBack = nullptr;

private:
    bool m_IsOnGround;
};