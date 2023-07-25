#pragma once
#include "Scene.h"
class MainScene : public Scene
{
public:
	MainScene();
	~MainScene();

public:
	bool Initialize(ID3D11Device* dev);
	bool Render(ID3D11DeviceContext* devcon);
	bool PostRender(ID3D11DeviceContext* devcon);
	virtual bool Update(ID3D11DeviceContext* devcon, InputClass* input, float fDeltaTime);

};

