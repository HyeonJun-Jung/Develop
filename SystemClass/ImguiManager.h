#pragma once
#include "..\stdafx.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx11.h"
#include "Imgui/imgui_impl_win32.h"

#include "Singleton.h"
/*
class ImguiManager
{
private: 
	ImguiManager();
	~ImguiManager();

private:
	static ImguiManager* m_Instance;

public:
	void Initialize(HWND, ID3D11Device*, ID3D11DeviceContext*);
	void Update();
	void BeginFrame();
	void EndFrame();

public:
	static ImguiManager* GetSingleton()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new ImguiManager();
		}

		return m_Instance;
	}
	
	float my_color[4] = {0, 0, 0, 0};
};
*/


class ImguiManager :public Singleton<ImguiManager>
{
public:
	ImguiManager();
	~ImguiManager();

public:
	void Initialize(HWND, ID3D11Device*, ID3D11DeviceContext*);
	void Update();
	void BeginFrame();
	void EndFrame();

public:
	float my_color[4] = { 0, 0, 0, 0 };

	HWND hwnd = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* devcon = nullptr;
};

