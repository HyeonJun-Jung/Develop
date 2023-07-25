#include "..\stdafx.h"
#include "GraphicsClass.h"
#include "CameraClass.h"
#include "D3DClass.h"

#include "TextClass.h"
#include "Terrain.h"
#include "SkydomeClass.h"

#include "ImguiManager.h"
#include "ModelManager.h"
#include "LightManager.h"
#include "SceneManager.h"

#include "TexturedObject.h"
#include "TexturedGraphicC.h"
#include "DynamicPhysicsC.h"

GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, InputClass* input)
{
    srand(time(NULL));
    bool result;

    m_Hwnd = &hwnd;
    m_Input = input;

    // Direct3D ��ü�� �ʱ�ȭ�մϴ�.
    result = D3DClass::GetSingleton()->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    // Physx �ʱ�ȭ
    if (!PhysicsComponent::InitPhysics())
        return false;

    if (!ImguiManager::GetSingleton()->Initialize(hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext()))
        return false;

    if (!LightManager::GetSingleton()->Initialize(D3DClass::GetSingleton()->GetDevice()))
        return false;

    if (!ModelManager::GetSingleton()->Initialize(hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext()))
        return false;

    // ī�޶� ������ ����
    CameraClass::GetSingleton()->SetPosition(0.f, 0.f, -10.0f);

    // ī�޶� ȸ�� ����
    CameraClass::GetSingleton()->SetRotation(0.0f, 0.0f, 0.0f);
    CameraClass::GetSingleton()->Render();

    InitializeModel(screenWidth, screenHeight, hwnd);

    // �𵨵��� ��� �ʱ�ȭ�� �Ŀ� ���� ������  
    LightManager::GetSingleton()->RenderLights(D3DClass::GetSingleton()->GetDeviceContext());
    
    if (!SceneManager::GetSingleton()->Initialize(D3DClass::GetSingleton()->GetDevice(), input))
        return false;
        
    return true;
}

bool GraphicsClass::InitializeModel(int screenWidth, int screenHeight, HWND hwnd)
{
    // m_Text ��ü ����
    m_Text = new TextClass;
    if (!m_Text)
    {
        return false;
    }
    XMMATRIX baseMatrix; CameraClass::GetSingleton()->GetViewMatrix(baseMatrix);
    if (!m_Text->Initialize(D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseMatrix))
    {
        return false;
    }

    return true;
}


void GraphicsClass::Shutdown()
{
    if (m_Frustum)
    {
        delete m_Frustum;
        m_Frustum = 0;
    }

    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = 0;
    }

    return;
}


bool GraphicsClass::Frame(float fDeltaTime)
{
    m_fDeltaTime = fDeltaTime;

    
    if (!PhysicsComponent::UpdatePhysics(fDeltaTime))
    {
        return false;
    }
    if (!Update(fDeltaTime))
        return false;

    if (!Render())
        return false;

    return true;
}

bool GraphicsClass::Update(float fDeltaTime)
{
    for (int i = 0; i < m_TexturedList.size(); i++)
    {
        if (!m_TexturedList[i]->UpdatePhysics(D3DClass::GetSingleton()->GetDeviceContext(), fDeltaTime))
            return false;
    }
    
    
    if (!SceneManager::GetSingleton()->Update(D3DClass::GetSingleton()->GetDeviceContext(), fDeltaTime))
        return false;
    

    return true;
}

bool GraphicsClass::Render()
{

    if (!RenderScene())
    {
        return false;
    }

    // RenderImGui();
  
    // ���ۿ� �׷��� ���� ȭ�鿡 ǥ���մϴ�.
    D3DClass::GetSingleton()->EndScene();

    return true;
}

bool GraphicsClass::RenderImGui()
{
    ImguiManager::GetSingleton()->BeginFrame();
    ImguiManager::GetSingleton()->Update();
    ImguiManager::GetSingleton()->EndFrame();
    return true;
}

bool GraphicsClass::RenderScene()
{
    static float rotation = 0.0f;
    rotation += (float)XM_PI * 0.0025f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    // �� �׸��⸦ �����ϱ� ���� ������ ������ ����ϴ�.
    //D3DClass::GetSingleton()->BeginScene(255.f, 255.f, 255.f, 1.f);
    D3DClass::GetSingleton()->BeginScene(0.f, 0.f, 0.f, 1.f);
    // ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
    CameraClass::GetSingleton()->Render();

    // ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
    D3DClass::GetSingleton()->GetWorldMatrix(worldMatrix);
    CameraClass::GetSingleton()->GetViewMatrix(viewMatrix);
    D3DClass::GetSingleton()->GetProjectionMatrix(projectionMatrix);
    D3DClass::GetSingleton()->GetOrthoMatrix(orthoMatrix);



    ///////////////////////
    ///////////////////////
    ////// 2D ������ //////
    ///////////////////////
    ///////////////////////
    
    // ��� 2D �������� �����ϱ� ���� Z���۸� ���ϴ�.
    D3DClass::GetSingleton()->TurnZBufferOff();

    // �ؽ�Ʈ�� �������ϱ� ���� ���� ������ �մϴ�. 
    D3DClass::GetSingleton()->TurnOnAlphaBlending();

    XMFLOAT3 CameraPosition = CameraClass::GetSingleton()->GetPosition();
    XMFLOAT3 LookAt = CameraClass::GetSingleton()->GetLookAt();
    
    if (!m_Text->SetCameraPosition(CameraPosition.x, CameraPosition.y, CameraPosition.z, D3DClass::GetSingleton()->GetDeviceContext()))
    {
        return false;
    }

    if (!m_Text->SetCameraLookAt(LookAt.x, LookAt.y, LookAt.z, D3DClass::GetSingleton()->GetDeviceContext()))
    {
        return false; 
    }

    // �ؽ�Ʈ�� �������մϴ�.
    if (!m_Text->Render(D3DClass::GetSingleton()->GetDeviceContext(), worldMatrix, orthoMatrix))
    {
        return false;
    }
    
    // �ؽ�Ʈ�� ������ �� �ٽ� ���ĺ����� ���ϴ�.
    D3DClass::GetSingleton()->TurnOffAlphaBlending();

    // ��� 2D �������� �Ϸ�Ǿ����� �ٽ� Z���۸� ŵ�ϴ�.
    D3DClass::GetSingleton()->TurnZBufferOn();

    ///////////////////////
    ///////////////////////
    ////// 3D ������ //////
    ///////////////////////
    ///////////////////////
    XMMATRIX SkyWorldMatrix = XMMatrixTranslation(CameraPosition.x, CameraPosition.y, CameraPosition.z);

    D3DClass::GetSingleton()->TurnZBufferOff();
    D3DClass::GetSingleton()->TurnOffCulling();

    // m_Skydome->Render();

    D3DClass::GetSingleton()->TurnZBufferOn();
    D3DClass::GetSingleton()->TurnOnCulling();

    // Camera Buffer, Light Buffer Setting
    LightManager::GetSingleton()->RenderLights(D3DClass::GetSingleton()->GetDeviceContext());
    
    if (!SceneManager::GetSingleton()->Render(D3DClass::GetSingleton()->GetDeviceContext()))
       return false;
        

    for (int i = 0; i < m_TexturedList.size(); i++)
    {
        if (!m_TexturedList[i]->Render(D3DClass::GetSingleton()->GetDeviceContext()))
            return false;
    }

    return true;
}

bool GraphicsClass::AddBox()
{
    string TexturedFileName = "../SystemClass/data/Box/Sci-fi Container Game Free.obj";

    TexturedGraphicC* texturedGraphic = new TexturedGraphicC();
    if (!texturedGraphic)
        return false;
    if (!texturedGraphic->Initialize(D3DClass::GetSingleton()->GetDevice()))
        return false;

    DynamicPhysicsC* physicsComponent = new DynamicPhysicsC();
    PxTransform transform = PxTransform(PxVec3(30.f, 10.f, 0));
    PxMaterial* material = PhysicsComponent::GetPhysics()->createMaterial(0.5, 0.5, 0.5);
    if (!physicsComponent->Initialize(transform, PxBoxGeometry(10.f, 10.f, 10.f), material, 10.f, 10.f))
        return false;

    TexturedObject* Box = new TexturedObject();
    if (!Box)
        return false;
    if (!Box->Initialize(TexturedFileName, texturedGraphic, physicsComponent))
        return false;

    m_TexturedList.push_back(Box);

    return true;
}