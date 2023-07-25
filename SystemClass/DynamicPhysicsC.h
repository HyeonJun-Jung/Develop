#pragma once
#include "PhysicsComponent.h"
class DynamicPhysicsC :
    public PhysicsComponent
{
public:
    DynamicPhysicsC();
    virtual ~DynamicPhysicsC();

public:
    bool Initialize(const PxTransform& t, const PxGeometry& geometry, PxMaterial* material, 
                    float density, float mass, const PxVec3& velocity = PxVec3(0));
    bool Update(Object* obj);

private:
    PxRigidDynamic* m_Dynamic = nullptr;
};

