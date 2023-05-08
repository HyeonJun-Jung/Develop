#pragma once
#include <map>
#include <string>
#include "Singleton.h"

class Scene;

class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

public:
	bool Init();

	bool Render();
	bool Update(float fDeltaTime);

private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;

};

