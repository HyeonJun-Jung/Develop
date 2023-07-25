#include "FieldLayer.h"
#include "..\stdafx.h"
#include "Player.h"
#include "SkinnedObject.h"
#include "TexturedObject.h"
#include "Terrain.h"

#include "SkinnedGraphicC.h"
#include "TexturedGraphicC.h"
#include "TerrainGraphicC.h"

#include "AnimationComponent.h"

#include "DynamicPhysicsC.h"
#include "StaticPhysicsC.h"

FieldLayer::FieldLayer(std::string layerName, int priority) : Layer(layerName, priority)
{
}

FieldLayer::~FieldLayer()
{
}

bool FieldLayer::Initialize(ID3D11Device* dev)
{
	if (!Layer::Initialize(dev))
		return false;


    SkinnedGraphicC* skinnedGraphic = new SkinnedGraphicC();
    if (!skinnedGraphic)
        return false;
    if (!skinnedGraphic->Initialize(dev))
        return false;

    TexturedGraphicC* texturedGraphic = new TexturedGraphicC();
    if (!texturedGraphic)
        return false;
    if (!texturedGraphic->Initialize(dev))
        return false;

    AnimationComponent* animationComponent = new AnimationComponent;
    if (!animationComponent)
        return false;
    if (!animationComponent->Initialize(dev))
        return false;

    DynamicPhysicsC* BoxPhysicsC = new DynamicPhysicsC();
    PxTransform transform = PxTransform(PxVec3(30.f, 10.f, 0));
    PxMaterial* material = PhysicsComponent::GetPhysics()->createMaterial(0.5, 0.5, 0.5);
    if (!BoxPhysicsC->Initialize(transform, PxBoxGeometry(0.3f, 0.3f, 0.3f), material, 10.f, 10.f))
        return false;

    string WmodelFileName = "../SystemClass/data/Standard Walk.fbx";
    string TexturedFileName = "../SystemClass/data/Box/Sci-fi Container Game Free.obj";

    /*
    // SKinnedObj 持失
    SkinnedObject* m_SkinnedObj = new SkinnedObject();
    if (!m_SkinnedObj)  return false;
    if (!m_SkinnedObj->Initialize(WmodelFileName, skinnedGraphic, animationComponent))
        return false;
   
    m_ModelList.push_back(m_SkinnedObj);
    */
    
    // Player 持失
    Player* m_Player = new Player();
    if (!m_Player)  return false;
    m_Player->SetPosition(XMFLOAT3(0, 100, 0));

    if (!m_Player->Initialize(WmodelFileName, skinnedGraphic, animationComponent))
        return false;


    m_ModelList.push_back(m_Player);


    // TexturedObj 持失
    TexturedObject* m_TexturedObj = new TexturedObject();
    if (!m_TexturedObj)
        return false;
    if (!m_TexturedObj->Initialize(TexturedFileName, texturedGraphic, BoxPhysicsC))
        return false;

    m_ModelList.push_back(m_TexturedObj);

    // Terrain 持失
    char TerrainFileName[] = "../SystemClass/data/Terrain/hm.bmp";
    WCHAR TextureFileName[] = L"../SystemClass/data/Terrain/dirt.dds";
    Terrain* m_Terrain = new Terrain();
    //m_Terrain->SetScale(XMFLOAT3(5, 3, 5));
   // m_Terrain->SetPosition(XMFLOAT3(-100, 0, -100));
    if (!m_Terrain->Initialize(dev, TerrainFileName, TextureFileName))
    {
        return false;
    }

    m_ModelList.push_back(m_Terrain);

	return true;
}

bool FieldLayer::Render(ID3D11DeviceContext* devcon)
{
	if (!Layer::Render(devcon))
		return false;
	return true;
}

bool FieldLayer::PostRender(ID3D11DeviceContext* devcon)
{
    if (!Layer::PostRender(devcon))
        return false;
    return true;
}

bool FieldLayer::UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime)
{
    if (!Layer::UpdateAnimation(devcon, fDeltaTime))
        return false;
    return true;
}

bool FieldLayer::UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime)
{
    if (!Layer::UpdatePhysics(devcon, fDeltaTime))
        return false;
    return true;
}