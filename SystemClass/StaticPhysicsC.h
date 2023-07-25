#pragma once
#include "PhysicsComponent.h"
class StaticPhysicsC :
    public PhysicsComponent
{
public:
    StaticPhysicsC();
    virtual ~StaticPhysicsC();

public:
    bool Initialize(const PxTransform& t, const PxGeometry& geometry, PxMaterial* material);

    bool Update(Object* obj);

private:
    PxRigidStatic* m_Static = nullptr;
};

