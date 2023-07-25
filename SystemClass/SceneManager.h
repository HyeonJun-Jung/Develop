#pragma once
#include <map>
#include <string>
#include "Singleton.h"
#include "..\stdafx.h"

class Scene;
class InputClass;

class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

public:
	bool Initialize(ID3D11Device* dev, InputClass* input);

	bool Render(ID3D11DeviceContext* devcon);
	bool Update(ID3D11DeviceContext* devcon, float fDeltaTime);

private:
	Scene* m_currentScene = nullptr;
	Scene* m_nextScene = nullptr;
	InputClass* m_Input = nullptr;

};

