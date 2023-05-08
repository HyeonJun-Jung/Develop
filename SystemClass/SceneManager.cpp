#include "SceneManager.h"
#include "Scene.h"

SceneManager* SceneManager::m_Instance = nullptr;

SceneManager::SceneManager()
{
	
}

SceneManager::~SceneManager()
{
}

bool SceneManager::Init()
{
	// Create Scene //
	return true;
}

bool SceneManager::Render()
{
	if (!currentScene->Render())
		return false;
	
	return true;
}

bool SceneManager::Update(float fDeltaTime)
{
	if (!currentScene->Update(fDeltaTime))
		return false;

	return true;
}
