#pragma once
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class InputClass
{
	enum {
		NONE,
		DOWN,
		UP,
		PRESS
	};

public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	void SetMouse(LPARAM IParam);

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);

	bool Down(UINT key) { return m_keyState[key] == DOWN; }
	bool Up(UINT key) { return m_keyState[key] == UP; }
	bool Press(UINT key) { return m_keyState[key] == PRESS; }

	bool MouseDown(UINT key) { return m_mouseState[key] == DOWN; }
	bool MouseUp(UINT key) { return m_mouseState[key] == UP; }
	bool MousePress(UINT key) { return m_mouseState[key] == PRESS; }

	IDirectInputDevice8* GetMouse() { return m_mouse; }
	IDirectInputDevice8* GetKeyBoard() { return m_keyboard; }

	bool IsNumber1Pressed();

	bool IsCameraRotationRight();
	bool IsCameraRotationLeft();
	bool IsCameraRotationUp();
	bool IsCameraRotationDown();

	bool IsCameraZoomIn();
	bool IsCameraZoomOut();
	bool IsCameraMoveRight();
	bool IsCameraMoveLeft();

	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();
	bool IsMouseLeftClicked();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

public:
	unsigned char m_oldkeyboardState[256];
	unsigned char m_curkeyboardState[256];
	unsigned char m_keyState[256];

	DIMOUSESTATE m_oldmouseState;
	DIMOUSESTATE m_curmouseState;
	unsigned char m_mouseState[4];

	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_mouseX = 0;
	int m_mouseY = 0;
    
};

#endif

