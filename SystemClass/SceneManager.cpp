#include "SceneManager.h"
#include "Scene.h"
#include "MainScene.h"
SceneManager* SceneManager::m_Instance = nullptr;

SceneManager::SceneManager()
{
	
}

SceneManager::~SceneManager()
{
}

bool SceneManager::Initialize(ID3D11Device* dev, InputClass* input)
{
	m_Input = input;

	// Create Scene //
	MainScene* mainScene = new MainScene();
	if (!mainScene->Initialize(dev))
		return false;

	m_currentScene = mainScene;

	return true;
}

bool SceneManager::Render(ID3D11DeviceContext* devcon)
{
	if (!m_currentScene->Render(devcon))
		return false;
	/*
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	*/
	if (!m_currentScene->PostRender(devcon))
		return false;
	/*
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::EndFrame();
	*/
	return true;
}

bool SceneManager::Update(ID3D11DeviceContext* devcon, float fDeltaTime)
{
	if (!m_currentScene->Update(devcon, m_Input, fDeltaTime))
		return false;

	return true;
}
