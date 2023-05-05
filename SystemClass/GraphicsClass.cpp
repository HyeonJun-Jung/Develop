#include "GraphicsClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "D3DClass.h"
#include "TextureShaderClass.h"
#include "BitmapClass.h"
#include "TextClass.h"
#include "ModelListClass.h"
#include "FrustumClass.h"
#include "AssimpTest/ModelLoader.h"
#include "BumpMapShaderClass.h"
#include "ReflectionShaderClass.h"
#include "RenderTextureClass.h"
#include "TerrainClass.h"
#include "ImguiManager.h"
#include "Colorshaderclass.h"
#include "TerrainShaderClass.h"
#include "SkydomeClass.h"
#include "SkydomeShaderClass.h"
#include "TerrainReflectionShaderClass.h"
#include "ModelManager.h"
#include "WModel.h"
#include "WaterShaderClass.h"

#include "fbxsdk.h"

GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;

    m_Hwnd = &hwnd;

    // Direct3D ��ü�� �ʱ�ȭ�մϴ�.
    result = D3DClass::GetSingleton()->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    CameraClass::GetSingleton()->SetPosition(-10.0f, 0.0f, -30.0f);
    CameraClass::GetSingleton()->Render();
    
    // ī�޶� ������ ����
    CameraClass::GetSingleton()->SetPosition(0.f, 0.f, -10.0f);
    //m_Camera->SetPosition(0.0f, 0.0f, -10.f);
    // ī�޶� ȸ�� ����
    CameraClass::GetSingleton()->SetRotation(0.0f, 0.0f, 0.0f);

    InitializeShader(hwnd);
    InitializeModel(screenWidth, screenHeight, hwnd);

    return true;
}

bool GraphicsClass::InitializeShader(HWND hwnd)
{
    ///////////////////////
   ///////////////////////
   //////   Shader  //////
   ///////////////////////
   ///////////////////////

   // m_LightShader ��ü ����
    m_LightShader = new LightShaderClass;
    if (!m_LightShader)
    {
        return false;
    }

    // m_LightShader ��ü �ʱ�ȭ
    if (!m_LightShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not intialize the Light Shader object.", L"Error", MB_OK);
        return false;
    }

    // m_BumpMapShader ��ü ����
    m_BumpMapShader = new BumpMapShaderClass;
    if (!m_BumpMapShader)
    {
        return false;
    }

    // m_BumpMapShader ��ü �ʱ�ȭ
    if (!m_BumpMapShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not intialize the Bump Map Shader object.", L"Error", MB_OK);
        return false;
    }

    // m_ColorShader ��ü �ʱ�ȭ
    m_ColorShader = new ColorShaderClass;
    if (!m_ColorShader)
        return false;

    if (!m_ColorShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not intialize the Bump Map Shader object.", L"Error", MB_OK);
        return false;
    }

    m_TerrainShader = new TerrainShaderClass;
    if (!m_TerrainShader)
        return false;

    if (!m_TerrainShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
        return false;

    m_TerrainReflectionShader = new TerrainReflectionShaderClass;
    if (!m_TerrainReflectionShader)
        return false;

    if (!m_TerrainReflectionShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
        return false;

    m_SkydomeShader = new SkydomeShaderClass;
    if (!m_SkydomeShader)
        return false;

    if (!m_SkydomeShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
    {
        return false;
    }

    string ModelDir = "../SystemClass/data/Standard Walk.fbx";

    if (!ModelManager::GetSingleton()->LoadModel(ModelDir, hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext()))
        return false;



    // m_TextureShader ��ü ����
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // m_TextureShader ��ü �ʱ�ȭ
    if (!m_TextureShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
    {
        return false;
    }





    // m_ReflectionShader ��ü ����
    m_ReflectionShader = new ReflectionShaderClass;
    if (!m_ReflectionShader)
    {
        return false;
    }

    // m_ReflectionShader ��ü �ʱ�ȭ
    if (!m_ReflectionShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
    {
        return false;
    }

    m_WaterShader = new WaterShaderClass;
    if (!m_WaterShader)
        return false;

    if (!m_WaterShader->Initialize(D3DClass::GetSingleton()->GetDevice(), hwnd))
        return false;

    ImguiManager::GetSingleton()->Initialize(hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext());

    

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
    // m_Text ��ü �ʱ�ȭ
    if (!m_Text->Initialize(D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseMatrix))
    {
        return false;
    }

    // m_Model ��ü ����
    m_Model = new ModelClass;
    if (!m_Model)
        return false;

    WCHAR textureFilename[] = L"../SystemClass/data/seafloor2.dds";
    char ModelFilename[] = "../SystemClass/data/test.txt";
    // m_Model ��ü �ʱ�ȭ
    if (!m_Model->Initialize(hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext(), ModelFilename, textureFilename))
    {
        MessageBox(hwnd, L"Could not intialize the model object.", L"Error", MB_OK);
        return false;
    }

    string FbxFilename = "../SystemClass/data/Standard Walk.fbx";
    if (!ModelManager::GetSingleton()->LoadModel(FbxFilename, hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext()))
        return false;

    FbxFilename = "../SystemClass/data/Quick Roll To Run.fbx";
    if (!ModelManager::GetSingleton()->LoadModel(FbxFilename, hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext()))
        return false;


    FbxFilename = "../SystemClass/data/Testwuson.X";
    if (!ModelManager::GetSingleton()->LoadModel(FbxFilename, hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext()))
        return false;

    FbxFilename = "../SystemClass/data/Jumping.fbx";
    if (!ModelManager::GetSingleton()->LoadModel(FbxFilename, hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext()))
        return false;

    // Wmodel ����
    m_Wmodel = new WModel;
    if (!m_Wmodel)
        return false;

    string WmodelFileName = "../SystemClass/data/Standard Walk.fbx";

    if (!m_Wmodel->LoadWModel(WmodelFileName))
        return false;

    if (!m_Wmodel->SetShader(m_BumpMapShader))
        return false;

    m_FloorModel = new ModelClass;
    if (!m_FloorModel)
    {
        return false;
    }

    WCHAR FloorTextureFilename[] = L"../SystemClass/data/water01.dds";
    char FloorModelFilename[] = "../SystemClass/data/floor.txt";
    if (!m_FloorModel->Initialize(hwnd, D3DClass::GetSingleton()->GetDevice(), D3DClass::GetSingleton()->GetDeviceContext(), FloorModelFilename, FloorTextureFilename))
    {
        return false;
    }

    // m_Light ��ü ����
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // m_Light ��ü �ʱ�ȭ
    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(1.0f, 0.0f, 1.0f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(32.0f);



    char TerrainFileName[] = "../SystemClass/data/Terrain/hm.bmp";
    WCHAR TextureFileName[] = L"../SystemClass/data/Terrain/dirt.dds";
    m_Terrain = new TerrainClass;

    if (!m_Terrain->Initialize(D3DClass::GetSingleton()->GetDevice(), TerrainFileName, TextureFileName))
    {
        MessageBox(hwnd, L"Could not intialize the Terrain object.", L"Error", MB_OK);
    }

    m_Skydome = new SkydomeClass;
    if (!m_Skydome)
        return false;

    if (!m_Skydome->Initialize(D3DClass::GetSingleton()->GetDevice()))
        return false;


    // m_RenderTexture ��ü ����
    m_ReflectionRenderTexture = new RenderTextureClass;
    if (!m_ReflectionRenderTexture)
    {
        return false;
    }

    // m_RenderTexture ��ü �ʱ�ȭ
    if (!m_ReflectionRenderTexture->Initialize(D3DClass::GetSingleton()->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
    {
        return false;
    }

    // m_RenderTexture ��ü ����
    m_RefractionRenderTexture = new RenderTextureClass;
    if (!m_RefractionRenderTexture)
    {
        return false;
    }

    // m_RenderTexture ��ü �ʱ�ȭ
    if (!m_RefractionRenderTexture->Initialize(D3DClass::GetSingleton()->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
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

    if (m_ModelList)
    {
        m_ModelList->Shutdown();
        delete m_ModelList;
        m_ModelList = 0;
    }

    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = 0;
    }

    // m_Bitmap ��ü ��ȯ
    if (m_Bitmap)
    {
        m_Bitmap->Shutdown();
        delete m_Bitmap;
        m_Bitmap = 0;
    }

    // m_TextureShader ��ü ��ȯ
    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    if (m_BumpMapShader)
    {
        m_BumpMapShader->Shutdown();
        delete m_BumpMapShader;
        m_BumpMapShader = 0;
    }

    // m_Light ��ü ��ȯ
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // m_LightShader ��ü ��ȯ
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // m_Model ��ü ��ȯ
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    // Set the height of the water.
    m_waterHeight = 3.5f;

    // Initialize the position of the water.
    m_waterTranslation = 0.0f;

    return;
}


bool GraphicsClass::Frame(float fDeltaTime)
{
    m_fDeltaTime = fDeltaTime;

    

    // Update the position of the water to simulate motion.
    m_waterTranslation += 0.001f;
    if (m_waterTranslation > 1.0f)
    {
        m_waterTranslation -= 1.0f;
    }

    if (!Update(fDeltaTime))
        return false;

    if (!Render())
        return false;

    

    return true;
}

bool GraphicsClass::Update(float fDeltaTime)
{
    if (!m_Wmodel->Update(fDeltaTime))
    {
        return false;
    }
}

bool GraphicsClass::Render()
{  
    if (!RenderReflectionToTexture())
    {
        return false;
    }
    
    if (!RenderRefractionToTexture())
    {
        return false;
    }

    if (!RenderScene())
    {
        return false;
    }

    RenderImGui();

  
    // ���ۿ� �׷��� ���� ȭ�鿡 ǥ���մϴ�.
    D3DClass::GetSingleton()->EndScene();
    

    return true;
}

bool GraphicsClass::RenderImGui()
{
    ImguiManager::GetSingleton()->BeginFrame();
    ImguiManager::GetSingleton()->Update();
    ImguiManager::GetSingleton()->EndFrame();
    return false;
}


bool GraphicsClass::RenderReflectionToTexture()
{
    XMMATRIX WorldMatrix, ReflectionMatrix, ProjectionMatrix;

    m_ReflectionRenderTexture->SetRenderTarget(D3DClass::GetSingleton()->GetDeviceContext());
    m_ReflectionRenderTexture->ClearRenderTarget(D3DClass::GetSingleton()->GetDeviceContext(), 0.0, 0.0, 0.0, 1.0);

    CameraClass::GetSingleton()->ReflectionRender(3.5);

    CameraClass::GetSingleton()->GetReflectionViewMatrix(ReflectionMatrix);
    D3DClass::GetSingleton()->GetWorldMatrix(WorldMatrix);
    D3DClass::GetSingleton()->GetProjectionMatrix(ProjectionMatrix);

    static float rotation = 0.0f;
    rotation += (float)XM_PI * 0.0025f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }
        // ī�޶� ��ġ�� ��´�.
    XMFLOAT3 cameraPosition = CameraClass::GetSingleton()->GetPosition();

    // �ݻ� �� ī�޶� ��ġ�� ���� ��� ������ ī�޶��� Y ��ǥ�� �������ϴ�.
    cameraPosition.y = -cameraPosition.y + (3.5 * 2.0f);

    XMMATRIX SkyWorldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

    D3DClass::GetSingleton()->TurnZBufferOff();
    D3DClass::GetSingleton()->TurnOffCulling();

    m_Skydome->Render(D3DClass::GetSingleton()->GetDeviceContext());
    if (!m_SkydomeShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), m_Skydome->GetIndexCount(), SkyWorldMatrix, ReflectionMatrix, ProjectionMatrix,
        m_Skydome->GetApexColor(), m_Skydome->GetCenterColor()))

    {
        return false;
    }

    D3DClass::GetSingleton()->TurnZBufferOn();
    D3DClass::GetSingleton()->TurnOnCulling();
    
    XMFLOAT4 ClipPlane = XMFLOAT4(0.0f, 1.0f, 0.0f, -3.5);
    m_Terrain->Render(D3DClass::GetSingleton()->GetDeviceContext());


    if(!m_TerrainReflectionShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), m_Terrain->GetIndexCount(), WorldMatrix, ReflectionMatrix, ProjectionMatrix, m_Terrain->GetTexture(),
         m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), ClipPlane))
         return false;



     // WorldMatrix = XMMatrixRotationY(rotation);    
     /*
     // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
     m_Model->Render(D3DClass::GetSingleton()->GetDeviceContext());

     // Render the model using the texture shader and the reflection view matrix.
     m_TextureShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ReflectionMatrix,
         ProjectionMatrix, m_Model->GetTexture());
         */

    

    D3DClass::GetSingleton()->SetBackBufferRenderTarget();
    D3DClass::GetSingleton()->ResetViewport();
    return true;
}

bool GraphicsClass::RenderRefractionToTexture()
{   
    XMMATRIX WorldMatrix, ViewMatrix, ProjectionMatrix;

    m_RefractionRenderTexture->SetRenderTarget(D3DClass::GetSingleton()->GetDeviceContext());
    m_RefractionRenderTexture->ClearRenderTarget(D3DClass::GetSingleton()->GetDeviceContext(), 255.0, 255.0, 255.0, 1.0);

    CameraClass::GetSingleton()->Render();
    D3DClass::GetSingleton()->GetWorldMatrix(WorldMatrix);
    CameraClass::GetSingleton()->GetViewMatrix(ViewMatrix);
    D3DClass::GetSingleton()->GetProjectionMatrix(ProjectionMatrix);

    XMFLOAT4 ClipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 3.5 + 0.1);

    m_Terrain->Render(D3DClass::GetSingleton()->GetDeviceContext());

    if (!m_TerrainReflectionShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), m_Terrain->GetIndexCount(), WorldMatrix, ViewMatrix, ProjectionMatrix, m_Terrain->GetTexture(),
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), ClipPlane))
        return false;

    

    D3DClass::GetSingleton()->SetBackBufferRenderTarget();
    D3DClass::GetSingleton()->ResetViewport();

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
    D3DClass::GetSingleton()->BeginScene(255.f, 255.f, 255.f, 1.f);

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

    m_Skydome->Render(D3DClass::GetSingleton()->GetDeviceContext());
    if (!m_SkydomeShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), m_Skydome->GetIndexCount(), SkyWorldMatrix, viewMatrix, projectionMatrix,
        m_Skydome->GetApexColor(), m_Skydome->GetCenterColor()))

    {
        return false;
    }

    D3DClass::GetSingleton()->TurnZBufferOn();
    D3DClass::GetSingleton()->TurnOnCulling();

    // Camera Buffer, Light Buffer Setting
    if (!m_BumpMapShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), CameraPosition,
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
        return false;

     m_Wmodel->PlayAnimation(0);

    if (!m_Wmodel->Render())
    {
        return false;
    }
    
    m_Terrain->Render(D3DClass::GetSingleton()->GetDeviceContext());
    if (!m_TerrainShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Terrain->GetTexture(),
                                   m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection()))
    {
        return false;
    }


    D3DClass::GetSingleton()->GetWorldMatrix(worldMatrix);
    worldMatrix = XMMatrixTranslation(240.0f, 3.5f, 250.0f);

    XMMATRIX ReflectionMatrix;
   
    CameraClass::GetSingleton()->GetReflectionViewMatrix(ReflectionMatrix);

    m_FloorModel->Render(D3DClass::GetSingleton()->GetDeviceContext());   

    if (!m_WaterShader->Render(D3DClass::GetSingleton()->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, ReflectionMatrix, m_ReflectionRenderTexture->GetShaderResourceView(),
        m_RefractionRenderTexture->GetShaderResourceView(), m_FloorModel->GetTexture(),
        m_waterTranslation, 0.01f))
        return false;

    return true;
}

