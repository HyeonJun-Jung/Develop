#include "TexturedObject.h"
#include "ModelManager.h"
#include "TexturedGraphicC.h"
#include "PhysicsComponent.h"
#include "DynamicPhysicsC.h"
TexturedObject::TexturedObject()
{
}

TexturedObject::~TexturedObject()
{
}

bool TexturedObject::Initialize(string fileName, TexturedGraphicC* graphicC, PhysicsComponent* physicsC)
{
	m_SkinnedModel = ModelManager::GetSingleton()->GetModel(fileName);
	if (!m_SkinnedModel)
		return false;

	m_GraphicC = graphicC;
	m_PhysicsC = physicsC;

    return true;
}

bool TexturedObject::UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime)
{
	if (!m_PhysicsC->Update(this))
		return false;

	return true;
}

bool TexturedObject::Render(ID3D11DeviceContext* devcon)
{
	if (!m_GraphicC->Update(devcon, this))
		return false;
    return true;
}
