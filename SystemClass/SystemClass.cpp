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
    // �ý��� ��踦 ������Ʈ �մϴ�.
    m_Fps->Frame();
    m_Cpu->Frame();
    m_Timer->Frame();

    bool result;


    int mouseX, mouseY;

    // �Է� �������� ó���� �����մϴ�.
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


    // �޼��� ����ü�� �ʱ�ȭ�մϴ�.
    ZeroMemory(&msg, sizeof(MSG));

    while (true)
    {
        // ������ �޼����� ó���մϴ�.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // ���� �޽����� ���� ��� �޽��� ������ Ż���մϴ�.
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // �� �ܿ��� Frame �Լ��� ó���մϴ�.
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
        // �����찡 ���ŵǾ����� Ȯ���մϴ�.
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }

    // �����찡 �������� Ȯ���մϴ�.
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }

    // �ٸ� ��� �޼������� system Ŭ������ �޼��� ó���⿡ �����մϴ�.
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


    // �Լ��� ���̿� �ʺ� �����ϱ� ���� ������ 0���� �ʱ�ȭ�Ѵ�.
    screenWidth = 0;
    screenHeight = 0;

    // �������� api�� ����Ͽ� �ʱ�ȭ�Ѵ�.
    InitializeWindows(screenWidth, screenHeight);

    // input ��ü�� �����մϴ�.  �� ��ü�� �����κ��� ������ Ű���� �Է��� ó���ϱ� ���� ����մϴ�.
    m_Input = new InputClass;
    if (!m_Input)
    {
        return false;
    }

    // Input ��ü�� �ʱ�ȭ�մϴ�.
    m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);

    // graphics ��ü�� �����մϴ�. �� ��ü�� �� ���ø����̼��� ��� �׷��� ��Ҹ� �׸��� ���� �մϴ�.
    m_Graphics = new GraphicsClass;
    if (!m_Graphics)
    {
        return false;
    }

    // graphics ��ü�� �ʱ�ȭ�մϴ�.
    result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
    if (!result)
    {
        return false;
    }

    // Camera ��ü�� �޾ƿɴϴ�.
    m_Camera = m_Graphics->GetCamera();
    

    // Fps ��ü�� �����մϴ�.
    m_Fps = new FpsClass;
    if (!m_Fps)
    {
        return false;
    }

    // Fps ��ü�� �ʱ�ȭ�մϴ�.
    m_Fps->Initialize();

    // Cpu ��ü�� �����մϴ�.
    m_Cpu = new CpuClass;
    if(!m_Cpu)
    {
        return false;
    }
    // Cpu ��ü�� �ʱ�ȭ�մϴ�.
    m_Cpu->Initialize();

    // Timer ��ü�� �����մϴ�.
    m_Timer = new TimerClass;
    if (!m_Timer)
    {
        return false;
    }

    // Timer ��ü�� �ʱ�ȭ�մϴ�.
    if (!m_Timer->Initialize())
    {
        MessageBox(m_hwnd, L"Could not initialize the Timer object", L"Error", MB_OK);
        return false;
    }
    
    // Position ��ü�� �����մϴ�.
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


    // �ܺ� �����͸� �� ��ü�� �����մϴ�.
    ApplicationHandle = this;

    // �� ���ø����̼��� �ν��Ͻ��� �����ɴϴ�.
    m_hinstance = GetModuleHandle(NULL);

    // ���ø����̼��� �̸��� �����մϴ�.
    m_applicationName = L"Practice";

    // ������ Ŭ������ �⺻ �������� ����ϴ�.
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

    // ������ Ŭ������ ����մϴ�.
    RegisterClassEx(&wc);

    // ����� ȭ���� �ػ󵵸� �˾ƿɴϴ�.
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Ǯ��ũ�� ��� ������ ���� ���� ȭ�� ������ �մϴ�.
    if (FULL_SCREEN)
    {
        // ���� Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũ�� ũ�⿡ ���߰� ������ 32bit�� �մϴ�.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Ǯ��ũ���� �´� ���÷��� ������ �մϴ�.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // �������� ��ġ�� ȭ���� ���� ���� ����ϴ�.
        posX = posY = 0;
    }
    else
    {
        // ������ ����� 800x600�� ũ�⸦ ������ �մϴ�.
        screenWidth = 800;
        screenHeight = 600;

        // â�� ������� �߾ӿ� ������ �մϴ�.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // ������ ���� ������ â�� ����� �� �ڵ��� �����ɴϴ�.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

    // �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ݴϴ�.
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    // ���콺 Ŀ���� ǥ������ �ʽ��ϴ�.
    ShowCursor(true);

    return;
}

void SystemClass::Shutdown()
{
    // Camera ��ü�� ��ȯ�մϴ�.
    if (m_Camera)
    {
        m_Camera = 0;
    }

    // Graphics ��ü�� ��ȯ�մϴ�.
    if (m_Graphics)
    {
        m_Graphics->Shutdown();
        delete m_Graphics;
        m_Graphics = 0;
    }

    // Input ��ü�� ��ȯ�մϴ�.
    if (m_Input)
    {   
        m_Input->Shutdown();
        delete m_Input;
        m_Input = 0;
    }

    // Fps ��ü�� ��ȯ�մϴ�.
    if (m_Fps)
    {
        delete m_Fps;
        m_Fps = 0;
    }

    // Cpu ��ü�� ��ȯ�մϴ�.
    if (m_Cpu)
    {
        m_Cpu->Shutdown();
        delete m_Cpu;
        m_Cpu = 0;
    }

    // Timer ��ü�� ��ȯ�մϴ�.
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

    // â�� �����ŵ�ϴ�.
    ShutdownWindows();

    return;
}


void SystemClass::ShutdownWindows()
{
    // ���콺 Ŀ���� ǥ���մϴ�.
    ShowCursor(true);

    // Ǯ��ũ�� ��带 �������� �� ���÷��� ������ �ٲߴϴ�.
    if (FULL_SCREEN)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    // â�� �����մϴ�.
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    // ���ø����̼� �ν��Ͻ��� �����մϴ�.
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = NULL;

    // �� Ŭ������ ���� �ܺ� ������ ������ �����մϴ�.
    ApplicationHandle = NULL;

    return;
}
