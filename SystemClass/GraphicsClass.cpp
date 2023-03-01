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
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx11.h"
#include "Imgui/imgui_impl_win32.h"

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




    // Direct3D 객체를 생성합니다.
    m_Direct3D = new D3DClass;
    if (!m_Direct3D)
    {
        return false;
    }

    // Direct3D 객체를 초기화합니다.
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }


    // Imgui 생성
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
    ImGui::StyleColorsDark();


    // m_Camera 객체 생성
    m_Camera = new CameraClass;
    if (!m_Camera)
        return false;

    XMMATRIX baseMatrix;
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->Render();
    m_Camera->GetViewMatrix(baseMatrix);

    // 카메라 포지션 설정
    m_Camera->SetPosition(-50.0f, 100.0f, -200.0f);
    //m_Camera->SetPosition(0.0f, 0.0f, -10.f);
    // 카메라 회전 설정
    m_Camera->SetRotation(0.0f, 0.0f, 0.0f);



    // m_Text 객체 생성
    m_Text = new TextClass;
    if (!m_Text)
    {
        return false;
    }

    // m_Text 객체 초기화
    if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseMatrix))
    {
        return false;
    }



    // m_Model 객체 생성
    m_Model = new ModelClass;
    if (!m_Model)
        return false;

    WCHAR textureFilename[] = L"../SystemClass/data/seafloor2.dds";
    //char ModelFilename[] = "../SystemClass/data/Standard Walk.fbx";
    char ModelFilename[] = "../SystemClass/data/test.txt";
    // m_Model 객체 초기화
    if (!m_Model->Initialize(hwnd, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), ModelFilename, textureFilename))
    {
        MessageBox(hwnd, L"Could not intialize the model object.", L"Error", MB_OK);
        return false;
    }

    
    m_FloorModel = new ModelClass;
    if (!m_FloorModel)
    {
        return false;
    }

    WCHAR FloorTextureFilename[] = L"../SystemClass/data/blue01.dds";
    char FloorModelFilename[] = "../SystemClass/data/floor.txt";
    if (!m_FloorModel->Initialize(hwnd, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), FloorModelFilename, FloorTextureFilename))
    {
        return false;
    }


    m_ModelLoader = new ModelLoader;
    if (!m_ModelLoader)
    {
        return false;
    }


    char FbxFilename[] = "../SystemClass/data/Standard Walk.fbx";
    // char FbxFilename[] = "../SystemClass/data/boblampclean.md5mesh";

    static float ModelanimationTime = 0.0f;
    ModelanimationTime += m_fDeltaTime;
    if (ModelanimationTime > 100)
    {
        ModelanimationTime -= 50;
    }

    if (!m_ModelLoader->Load(hwnd, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), ModelanimationTime, FbxFilename))
    {
        MessageBox(hwnd, L"Could not intialize the ModelLoader object.", L"Error", MB_OK);
        return false;
    }



    // m_TextureShader 객체 생성
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // m_TextureShader 객체 초기화
    if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
    {
        return false;
    }

    
    // m_LightShader 객체 생성
    m_LightShader = new LightShaderClass;
    if (!m_LightShader)
    {
        return false;
    }

    // m_LightShader 객체 초기화
    if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not intialize the Light Shader object.", L"Error", MB_OK);
        return false;
    }


    // m_BumpMapShader 객체 생성
    m_BumpMapShader = new BumpMapShaderClass;
    if (!m_BumpMapShader)
    {
        return false;
    }

    // m_BumpMapShader 객체 초기화
    if(!m_BumpMapShader->Initialize(m_Direct3D->GetDevice(), hwnd))
    { 
        MessageBox(hwnd, L"Could not intialize the Bump Map Shader object.", L"Error", MB_OK);
        return false;
    }




    // m_ReflectionShader 객체 생성
    m_ReflectionShader = new ReflectionShaderClass;
    if (!m_ReflectionShader)
    {
        return false;
    }

    // m_ReflectionShader 객체 초기화
    if (!m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd))
    {
        return false;
    }

    // m_RenderTexture 객체 생성
    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture)
    {
        return false;
    }

    // m_RenderTexture 객체 초기화
    if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
    {
        return false;
    }




    // m_Light 객체 생성
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // m_Light 객체 초기화
    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(1.0f, 0.0f, 1.0f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(32.0f);




    // ModelList 객체 생성
    m_ModelList = new ModelListClass;
    if (!m_ModelList)
    {
        return false;
    }

    if (!m_ModelList->Initialize(25))
    {
        MessageBox(hwnd, L"Could not initialize the ModelList object.", L"Error", MB_OK);
    }

    // Frustum 객체 생성
    m_Frustum = new FrustumClass;
    if (!m_Frustum)
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

    // m_Bitmap 객체 반환
    if (m_Bitmap)
    {
        m_Bitmap->Shutdown();
        delete m_Bitmap;
        m_Bitmap = 0;
    }

    // m_TextureShader 객체 반환
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

    // m_Light 객체 반환
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // m_LightShader 객체 반환
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // m_Model 객체 반환
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    // m_Camera 객체 반환
    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // D3D 객체를 반환합니다.
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }



    return;
}


bool GraphicsClass::Frame(float fDeltaTime)
{
    m_fDeltaTime = fDeltaTime;
    return true;
}

bool GraphicsClass::Render()
{
    if (!RenderToTexture())
    {
        return false;
    }

    if (!RenderScene())
    {
        return false;
    }



    return true;
}


bool GraphicsClass::RenderToTexture()
{
    XMMATRIX WorldMatrix, ReflectionMatrix, ProjectionMatrix;

    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 255.0, 255.0, 0.0, 1.0);

    m_Camera->ReflectionRender(-1.5);
    m_Camera->GetReflectionViewMatrix(ReflectionMatrix);

    m_Direct3D->GetWorldMatrix(WorldMatrix);
    m_Direct3D->GetProjectionMatrix(ProjectionMatrix);

    static float rotation = 0.0f;
    rotation += (float)XM_PI * 0.0025f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    WorldMatrix = XMMatrixRotationY(rotation);

    /*

    m_Model->Render(m_Direct3D->GetDeviceContext());

    if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ReflectionMatrix, ProjectionMatrix, m_Model->GetTexture()))
    {
        return false;
    }
    */

    /*
    * 
    * if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ReflectionMatrix, ProjectionMatrix,
        m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
    {
        return false;
    }

    */

    if (!m_BumpMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ReflectionMatrix, ProjectionMatrix,
        NULL, m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
    {
        return false;
    }


    m_ModelLoader->Draw(m_Direct3D->GetDeviceContext());
    

    m_Direct3D->SetBackBufferRenderTarget();

    return true;
}

bool GraphicsClass::RenderScene()
{
    float positionX = 0.0f;
    float positionY = 0.0f;
    float positionZ = 0.0f;
    float radius = 1.0f;

    static float rotation = 0.0f;
    rotation += (float)XM_PI * 0.0025f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    XMFLOAT4 color;

    // 씬 그리기를 시작하기 위해 버퍼의 내용을 지웁니다.
    m_Direct3D->BeginScene(255.f, 255.f, 255.f, 1.f);

    // 카메라의 위치에 따라 뷰 행렬을 생성합니다.
    m_Camera->Render();

    // 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);



    ///////////////////////
    ///////////////////////
    ////// 2D 렌더링 //////
    ///////////////////////
    ///////////////////////

    // 모든 2D 렌더링을 시작하기 위해 Z버퍼를 끕니다.
    m_Direct3D->TurnZBufferOff();

    // 텍스트를 렌더링하기 전에 알파 블렌딩을 켭니다. 
    m_Direct3D->TurnOnAlphaBlending();

    XMFLOAT3 CameraPosition = m_Camera->GetPosition();
    XMFLOAT3 LookAt = m_Camera->GetLookAt();

    if (!m_Text->SetCameraPosition(CameraPosition.x, CameraPosition.y, CameraPosition.z, m_Direct3D->GetDeviceContext()))
    {
        return false;
    }

    if (!m_Text->SetCameraLookAt(LookAt.x, LookAt.y, LookAt.z, m_Direct3D->GetDeviceContext()))
    {
        return false; 
    }

    // 텍스트를 렌더링합니다.
    if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
    {
        return false;
    }

    // 텍스트를 렌더링 후 다시 알파블렌딩을 끕니다.
    m_Direct3D->TurnOffAlphaBlending();



    // 모든 2D 렌더링이 완료되었으면 다시 Z버퍼를 킵니다.
    m_Direct3D->TurnZBufferOn();




    ///////////////////////
    ///////////////////////
    ////// 3D 렌더링 //////
    ///////////////////////
    ///////////////////////

    //worldMatrix = XMMatrixRotationY(rotation);

    /*
    
    m_Model->Render(m_Direct3D->GetDeviceContext());

   

    if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
    {
        return false;
    }
    
    */

    /*
    if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture()))
    {
        return false;
    }
    */

    

    if (!m_BumpMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        NULL, m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
    {
        return false;
    }

    m_ModelLoader->Draw(m_Direct3D->GetDeviceContext());
    
   


    m_Direct3D->GetWorldMatrix(worldMatrix);
    worldMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f);

    XMMATRIX reflectionMatrix;
    m_Camera->GetReflectionViewMatrix(reflectionMatrix);

    m_FloorModel->Render(m_Direct3D->GetDeviceContext());


    if (!m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
                                    m_FloorModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), reflectionMatrix))
    {
        return false;
    }

    

    // start the Dear ImGui Frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Create ImGui Test Window
    ImGui::Begin("Test");
    ImGui::End();
    //Assemble Together Draw Data
    ImGui::Render();
    //Render Draw Data
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



    // 버퍼에 그려진 씬을 화면에 표시합니다.
    m_Direct3D->EndScene();






    /*
    m_ModelLoader->RenderBuffers(m_Direct3D->GetDeviceContext());

    m_LightShader->Render(m_Direct3D->GetDeviceContext(), 46968, worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GerTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

   // m_ModelLoader->Draw(m_Direct3D->GetDeviceContext());

    
    m_Model->Render(m_Direct3D->GetDeviceContext());

    m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GerTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
*/




    /*
    // 절두체를 만듭니다.
    m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

    // 렌더링 할 모델의 개수를 얻는다
    int modelCount = m_ModelList->GetModelCount();

    // 렌더링 된 모델의 개수를 초기화합니다.
    int renderCount = 0;

    for (int i = 0; i < modelCount; i++)
    {
        // 모델리스트에서 모델의 위치와 색상을 가져옵니다.
        m_ModelList->GetData(i, positionX, positionY, positionZ, color);

        if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius))
        {
            // 모델을 렌더링 할 위치로 이동시킵니다.
            worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

            // 모델의 정점, 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
            m_Model->Render(m_Direct3D->GetDeviceContext());

            // 라이트 쉐이더를 사용해 모델을 렌더링합니다.
            m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
              m_Model->GerTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), color,
                m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
            
            // 원래의 월드 매트릭스로 리셋
            m_Direct3D->GetWorldMatrix(worldMatrix);

            renderCount++;
        }
    }
   

    m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext());

    // 모든 2D 렌더링을 시작하기 위해 Z버퍼를 끕니다.
    m_Direct3D->TurnZBufferOff();

    // 텍스트를 렌더링하기 전에 알파 블렌딩을 켭니다. 
    m_Direct3D->TurnOnAlphaBlending();

    // 텍스트를 렌더링합니다.
    if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
    {
        return false;
    }

    // 텍스트를 렌더링 후 다시 알파블렌딩을 끕니다.
    m_Direct3D->TurnOffAlphaBlending();

    // 모든 2D 렌더링이 완료되었으면 다시 Z버퍼를 킵니다.
    m_Direct3D->TurnZBufferOn();

    // 버퍼에 그려진 씬을 화면에 표시합니다.
    m_Direct3D->EndScene();
    */

   /*
   













   ///////////////////////
   ///////////////////////
   ////// 2D 렌더링 //////
   ///////////////////////
   ///////////////////////
    
   // 모든 2D 렌더링을 시작하기 위해 Z버퍼를 끕니다.
    m_Direct3D->TurnZBufferOff();

    // 텍스트를 렌더링하기 전에 알파 블렌딩을 켭니다. 
    m_Direct3D->TurnOnAlphaBlending();
   
    // 비트맵 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
    if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 0))
    {
        return false;
    }

    // 텍스처 쉐이더로 비트맵을 렌더링합니다.
    if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
        worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture()))
    {
        return false;
    }

    // 텍스트를 렌더링합니다.
    if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
    {
        return false;
    }

    // 텍스트를 렌더링 후 다시 알파블렌딩을 끕니다.
    m_Direct3D->TurnOffAlphaBlending();

    // 모든 2D 렌더링이 완료되었으면 다시 Z버퍼를 킵니다.
    m_Direct3D->TurnZBufferOn();

    */



    ///////////////////////
    ///////////////////////
    ////// 3D 렌더링 //////
    ///////////////////////
    ///////////////////////

    //m_Model->Render(m_Direct3D->GetDeviceContext());

    /*
    if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
    {
        return false;
    }
    */

    /*
        m_ModelLoader->RenderBuffers(m_Direct3D->GetDeviceContext());

        if (!m_BumpMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
            NULL, m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
            m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
        {
            return false;
        }
        */
   
    // 삼각형이 회전할 수 있도록 회전 값으로 월드 행렬을 회전합니다.
    //worldMatrix = XMMatrixRotationY(rotation);
   // worldMatrix = XMMatrixRotationX(rotation);

    // 로컬 좌표를 월드 좌표로 변환합니다.
   // XMMATRIX trans = XMMatrixTranslation(-70, -10, -10);
   // worldMatrix = XMMatrixMultiply(worldMatrix, trans);
  
    return true;
}


CameraClass* GraphicsClass::GetCamera()
{
    return m_Camera;
}
