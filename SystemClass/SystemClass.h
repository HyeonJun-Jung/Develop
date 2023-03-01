#pragma once

#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

class InputClass;
class GraphicsClass;
class CpuClass;
class FpsClass;
class TimerClass;
class PositionClass;
class CameraClass;

class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&);
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;

    InputClass* m_Input = nullptr;
    GraphicsClass* m_Graphics;
    FpsClass* m_Fps = nullptr;
    CpuClass* m_Cpu = nullptr;
    TimerClass* m_Timer = nullptr;
    PositionClass* m_Position = nullptr;
    CameraClass* m_Camera = nullptr;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;

#endif