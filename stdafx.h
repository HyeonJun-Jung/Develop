// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define _WIN32_WINNT 0x0600


#include "targetver.h"


// #define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� ����
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

// STL ���
#include <string>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>

#include <memory>
#include <stdarg.h>


// ����� ���� ���
#include "SystemClass/Flag.h"

// Directx11 ���̺귯�� ���
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include "DxDefine.h"

// Imgui ���̺귯�� ��� 
#include "SystemClass/Imgui/imgui.h"
#include "SystemClass/Imgui/imgui_impl_dx11.h"
#include "SystemClass/Imgui/imgui_impl_win32.h"

