// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _WIN32_WINNT 0x0600


#include "targetver.h"


// #define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

// STL 헤더
#include <string>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>

#include <memory>
#include <stdarg.h>


// 사용자 정의 헤더
#include "SystemClass/Flag.h"

// Directx11 라이브러리 헤더
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include "DxDefine.h"

// Imgui 라이브러리 헤더 
#include "SystemClass/Imgui/imgui.h"
#include "SystemClass/Imgui/imgui_impl_dx11.h"
#include "SystemClass/Imgui/imgui_impl_win32.h"

