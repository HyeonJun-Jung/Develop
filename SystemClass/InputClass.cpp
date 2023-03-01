#include "InputClass.h"
InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
   
    // 마우스 커서의 위치 지정에 사용될 화면 크기를 설정합니다.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Direct Input 인터페이스를 초기화합니다.
    if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL)))
    {
        return false;
    }

    // 키보드의 Direct Input 인터페이스를 생성합니다.
    if (FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL)))
    {
        return false;
    }

    // 데이터의 형식을 설정합니다. 이 경우 키보드이므로 사전 정의 된 데이터 형식을 사용할 수 있습니다.
    if (FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)))
    {
        return false;
    }

    // 다른 프로그램과 공유되지 않도록 키보드에 대한 협조 수준을 설정합니다.
    if (FAILED(m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
    {
        return false;
    }

    // 키보드를 할당받습니다.
    if (FAILED(m_keyboard->Acquire()))
    {
        return false;
    }

    // 마우스의 Direct Input 인터페이스를 생성합니다.
    if (FAILED(m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL)))
    {
        return false;
    }

    // 사전 정의된 마우스의 데이터 형식을 마우스에 설정합니다.
    if (FAILED(m_mouse->SetDataFormat(&c_dfDIMouse)))
    {
        return false;
    }

    // 다른 프로그램과 공유될 수 있도록 마우스에 대한 협조 수준을 설정합니다.
    if (FAILED(m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
    {
        return false;
    }

    // 마우스를 할당받습니다.
    if (FAILED(m_mouse->Acquire()))
    {
        return false;
    }

    return true;
}

void InputClass::Shutdown()
{
    // 마우스를 반환합니다.
    if (m_mouse)
    {
        m_mouse->Unacquire();
        m_mouse->Release();
        m_mouse = 0;
    }

    // 키보드를 반환합니다.
    if (m_keyboard)
    {
        m_keyboard->Unacquire();
        m_keyboard->Release();
        m_keyboard = 0;
    }

    // m_directInput 객체를 반환합니다.
    if (m_directInput)
    {
        m_directInput->Release();
        m_directInput = 0;
    }
}

bool InputClass::Frame()
{
    // 키보드의 현재 상태를 읽습니다.
    if (!ReadKeyboard())
    {
        return false;
    }

    // 마우스의 현재 상태를 읽습니다.
    if (!ReadMouse())
    {
        return false;
    }

    // 키보드와 마우스의 변경 상태를 처리합니다.
    ProcessInput();

    return true;
}


bool InputClass::ReadKeyboard()
{
    HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
    
    if (FAILED(result))
    {
        // 키보드가 포커스를 잃었거나 획득되지 않을 경우 컨트롤을 다시 가져온다.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
        {
            m_keyboard->Acquire();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool InputClass::ReadMouse()
{
    HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);

    if (FAILED(result))
    {
        // 마우스가 포커스를 잃었거나 획득되지 않을 경우 컨트롤을 다시 가져온다.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
        {
            m_mouse->Acquire();
        }
        else
        {
            return false;
        }
    }

    return true;
}

void InputClass::ProcessInput()
{
    // 프레임동안 마우스 위치의 변경을 기반으로 마우스 커서의 위치를 업데이트합니다.
    m_mouseX += m_mouseState.lX;
    m_mouseY += m_mouseState.lY;

    
    // 마우스 위치가 화면 너비 또는 높이를 초과하지 않는지 확인합니다.
    if (m_mouseX < 0) { m_mouseX = 0; }
    if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth;}

    if (m_mouseY < 0) { m_mouseY = 0; }
    if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
    
}


bool InputClass::IsEscapePressed()
{
    // escape 키가 현재 눌러지고 있는지 bit값을 계산하여 확인한다.
    if (m_keyboardState[DIK_ESCAPE] * 0x80)
    {
        return true;
    }

    return false;
}

bool InputClass::IsLeftArrowPressed()
{
    if (m_keyboardState[DIK_A] * 0x80)
    {
        return true;
    }

    return false;
}

bool InputClass::IsRightArrowPressed()
{
    if (m_keyboardState[DIK_D] * 0x80)
    {
        return true;
    }

    return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
    mouseX = m_mouseX;
    mouseY = m_mouseY;
}

bool InputClass::IsCameraRotationRight()
{
    if (m_mouseState.rgbButtons[0] & 0x80)
    {
        if (m_mouseState.lX > 0)
            return true;
    }

    return false;
}

bool InputClass::IsCameraRotationLeft()
{
    if (m_mouseState.rgbButtons[0] & 0x80)
    {
        if (m_mouseState.lX < 0)
            return true;
    }

    return false;
}

bool InputClass::IsCameraRotationUp()
{
    if (m_mouseState.rgbButtons[0] & 0x80)
    {
        if (m_mouseState.lY < 0)
            return true;
    }

    return false;
}

bool InputClass::IsCameraRotationDown()
{
    if (m_mouseState.rgbButtons[0] & 0x80)
    {
        if (m_mouseState.lY > 0)
            return true;
    }

    return false;
}

bool InputClass::IsCameraZoomIn()
{
    if (m_mouseState.lZ > 0 || (m_keyboardState[DIK_W] & 0x80))
    {
        return true;
    }

    return false;
}

bool InputClass::IsCameraZoomOut()
{
    if (m_mouseState.lZ < 0 || (m_keyboardState[DIK_S] & 0x80))
    {
        return true;
    }

    return false;
}

bool InputClass::IsCameraMoveRight()
{
    if (m_keyboardState[DIK_D] & 0x80)
    {
        return true;
    }

    return false;
}

bool InputClass::IsCameraMoveLeft()
{
    if (m_keyboardState[DIK_A] & 0x80)
    {
        return true;
    }

    return false;
}
