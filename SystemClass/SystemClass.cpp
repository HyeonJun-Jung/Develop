#include "SystemClass.h"
#include "inputclass.h"
#include "graphicsclass.h"
#include "FpsClass.h"
#include "CpuClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "CameraClass.h"
#include "Flag.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx11.h"
#include "Imgui/imgui_impl_win32.h"

SystemClass::SystemClass()
{
}

SystemClass::SystemClass(const SystemClass&)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Frame()
{
    // 시스템 통계를 업데이트 합니다.
    m_Fps->Frame();
    m_Cpu->Frame();
    m_Timer->Frame();

    bool result;


    int mouseX, mouseY;

    // 입력 프레임의 처리를 수행합니다.
    if (!m_Input->Frame())
    {
        return false;
    }

    m_Position->SetFrameTime(m_Timer->GetTime());

    /*
    bool KeyDown = m_Input->IsLeftArrowPressed();
    m_Position->TurnLeft(KeyDown);

    KeyDown = m_Input->IsRightArrowPressed();
    m_Position->TurnRight(KeyDown);
    */

    if (m_Input->IsCameraRotationLeft())
    {
        m_Camera->CameraRotation(RT_LEFT);
    }

    if (m_Input->IsCameraRotationRight())
    {
        m_Camera->CameraRotation(RT_RIGHT);
    }

    if (m_Input->IsCameraRotationUp())
    {
        m_Camera->CameraRotation(RT_UP);
    }

    if (m_Input->IsCameraRotationDown())
    {
        m_Camera->CameraRotation(RT_DOWN);
    }

    if (m_Input->IsCameraZoomIn())
    {
        m_Camera->CameraMove(MD_FRONT);
    }

    if (m_Input->IsCameraZoomOut())
    {
        m_Camera->CameraMove(MD_BACK);
    }
    
    if (m_Input->IsCameraMoveLeft())
    {
        m_Camera->CameraMove(MD_LEFT);
    }

    if (m_Input->IsCameraMoveRight())
    {
        m_Camera->CameraMove(MD_RIGHT);
    }


    m_Graphics->Frame(m_Timer->GetTime());

    return m_Graphics->Render();
}

void SystemClass::Run()
{
    MSG msg;
    bool result;


    // 메세지 구조체를 초기화합니다.
    ZeroMemory(&msg, sizeof(MSG));

    while (true)
    {
        // 윈도우 메세지를 처리합니다.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // 종료 메시지를 받을 경우 메시지 루프를 탈출합니다.
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 그 외에는 Frame 함수를 처리합니다.
            result = Frame();
            if (!result)
            {
                MessageBox(m_hwnd, L"Frame processing Failed.", L"Error", MB_OK);
            }
        }

        if (m_Input->IsEscapePressed() == true)
        {
            break;
        }
    }

    return;
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
    {
        return true;
    }

    switch (umessage)
    {
        // 윈도우가 제거되었는지 확인합니다.
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }

    // 윈도우가 닫히는지 확인합니다.
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }

    // 다른 모든 메세지들은 system 클래스의 메세지 처리기에 전달합니다.
    default:
    {
        return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
    }
    }
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    return DefWindowProc(hwnd, umsg, wparam, lparam);
}

bool SystemClass::Initialize()
{
    int screenWidth, screenHeight;
    bool result;


    // 함수에 높이와 너비를 전달하기 전에 변수를 0으로 초기화한다.
    screenWidth = 0;
    screenHeight = 0;

    // 윈도우즈 api를 사용하여 초기화한다.
    InitializeWindows(screenWidth, screenHeight);

    // input 객체를 생성합니다.  이 객체는 유저로부터 들어오는 키보드 입력을 처리하기 이해 사용합니다.
    m_Input = new InputClass;
    if (!m_Input)
    {
        return false;
    }

    // Input 객체를 초기화합니다.
    m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);

    // graphics 객체를 생성합니다. 이 객체는 이 어플리케이션의 모든 그래픽 요소를 그리는 일을 합니다.
    m_Graphics = new GraphicsClass;
    if (!m_Graphics)
    {
        return false;
    }

    // graphics 객체를 초기화합니다.
    result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
    if (!result)
    {
        return false;
    }

    // Camera 객체를 받아옵니다.
    m_Camera = m_Graphics->GetCamera();
    

    // Fps 객체를 생성합니다.
    m_Fps = new FpsClass;
    if (!m_Fps)
    {
        return false;
    }

    // Fps 객체를 초기화합니다.
    m_Fps->Initialize();

    // Cpu 객체를 생성합니다.
    m_Cpu = new CpuClass;
    if(!m_Cpu)
    {
        return false;
    }
    // Cpu 객체를 초기화합니다.
    m_Cpu->Initialize();

    // Timer 객체를 생성합니다.
    m_Timer = new TimerClass;
    if (!m_Timer)
    {
        return false;
    }

    // Timer 객체를 초기화합니다.
    if (!m_Timer->Initialize())
    {
        MessageBox(m_hwnd, L"Could not initialize the Timer object", L"Error", MB_OK);
        return false;
    }
    
    // Position 객체를 생성합니다.
    m_Position = new PositionClass;
    if (!m_Position)
    {
        return false;
    }

    return true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;


    // 외부 포인터를 이 객체로 설정합니다.
    ApplicationHandle = this;

    // 이 어플리케이션의 인스턴스를 가져옵니다.
    m_hinstance = GetModuleHandle(NULL);

    // 어플리케이션의 이름을 설정합니다.
    m_applicationName = L"Practice";

    // 윈도우 클래스를 기본 설정으로 맞춥니다.
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // 윈도우 클래스를 등록합니다.
    RegisterClassEx(&wc);

    // 모니터 화면의 해상도를 알아옵니다.
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 풀스크린 모드 변수의 값에 따라 화면 설정을 합니다.
    if (FULL_SCREEN)
    {
        // 만약 풀스크린 모드라면 화면 크기를 데스크톱 크기에 맞추고 색상을 32bit로 합니다.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // 풀스크린에 맞는 디스플레이 설정을 합니다.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // 윈도우의 위치를 화면의 왼쪽 위로 맞춥니다.
        posX = posY = 0;
    }
    else
    {
        // 윈도우 모드라면 800x600의 크기를 가지게 합니다.
        screenWidth = 800;
        screenHeight = 600;

        // 창을 모니터의 중앙에 오도록 합니다.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // 설정한 것을 가지고 창을 만들고 그 핸들을 가져옵니다.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

    // 윈도우를 화면에 표시하고 포커스를 줍니다.
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    // 마우스 커서를 표시하지 않습니다.
    ShowCursor(true);

    return;
}

void SystemClass::Shutdown()
{
    // Camera 객체를 반환합니다.
    if (m_Camera)
    {
        m_Camera = 0;
    }

    // Graphics 객체를 반환합니다.
    if (m_Graphics)
    {
        m_Graphics->Shutdown();
        delete m_Graphics;
        m_Graphics = 0;
    }

    // Input 객체를 반환합니다.
    if (m_Input)
    {   
        m_Input->Shutdown();
        delete m_Input;
        m_Input = 0;
    }

    // Fps 객체를 반환합니다.
    if (m_Fps)
    {
        delete m_Fps;
        m_Fps = 0;
    }

    // Cpu 객체를 반환합니다.
    if (m_Cpu)
    {
        m_Cpu->Shutdown();
        delete m_Cpu;
        m_Cpu = 0;
    }

    // Timer 객체를 반환합니다.
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    if (m_Position)
    {
        delete m_Position;
        m_Position = 0;
    }

    // 창을 종료시킵니다.
    ShutdownWindows();

    return;
}


void SystemClass::ShutdownWindows()
{
    // 마우스 커서를 표시합니다.
    ShowCursor(true);

    // 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꿉니다.
    if (FULL_SCREEN)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    // 창을 제거합니다.
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    // 어플리케이션 인스턴스를 제거합니다.
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = NULL;

    // 이 클래스에 대한 외부 포인터 참조를 제거합니다.
    ApplicationHandle = NULL;

    return;
}
