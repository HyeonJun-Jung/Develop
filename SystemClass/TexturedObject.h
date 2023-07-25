#pragma once
#include "Object.h"
class TexturedGraphicC;
class SkinnedModel;
class PhysicsComponent;
class TexturedObject :
    public Object
{
public:
    TexturedObject();
    virtual ~TexturedObject();

    virtual bool Initialize(string fileName, TexturedGraphicC* graphicC, PhysicsComponent* physicsC);
    virtual bool UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime);
    virtual bool Render(ID3D11DeviceContext* devcon);
    SkinnedModel* GetSkinnedModel() { return m_SkinnedModel; }

private:
    TexturedGraphicC* m_GraphicC = nullptr;
    PhysicsComponent* m_PhysicsC = nullptr;
private:
    SkinnedModel* m_SkinnedModel = nullptr;

};

