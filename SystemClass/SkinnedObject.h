#pragma once
#include "Object.h"
#include "ModelStructure.h"

class SkinnedModel;
class SkinnedGraphicC;
class AnimationComponent;
class ControllerPhysicsC;
class InputClass;

class SkinnedObject :
    public Object
{
public:
    SkinnedObject();
    virtual ~SkinnedObject();

    virtual bool Initialize(string fileName, SkinnedGraphicC* graphicC, AnimationComponent* animationC);
    virtual bool UpdateInput(InputClass* input);
    virtual bool UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime);
    virtual bool UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime);
    virtual bool Render(ID3D11DeviceContext* devcon);

    virtual bool IsOnGround();
    
    void SetVelocity(XMFLOAT3 velocity) { m_Velocity = velocity; }
    void SetPhysxVelocity(PxVec3 velocity) { m_PhysxVelocity = velocity; }
    void SetNextAnimation(int animationIndex);
    void SetAnimationFrame(int frame);

    XMFLOAT3 GetVelocity() { return m_Velocity; }
    PxVec3 GetPhysxVelocityd() { return m_PhysxVelocity; }
    SkinnedModel* GetSkinnedModel() { return m_SkinnedModel; }

protected:
    AnimationComponent* m_AnimationC = nullptr;
    SkinnedGraphicC* m_GraphicC = nullptr;
    ControllerPhysicsC* m_PhysicsC = nullptr;
    PxVec3 m_PhysxVelocity = PxVec3(0, 0, 0);
    SkinnedModel* m_SkinnedModel = nullptr;

};

