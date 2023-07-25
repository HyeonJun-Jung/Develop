#pragma once
#include "Singleton.h"
#include "..\stdafx.h"

class SkinnedModel;
class ModelManager : public Singleton<ModelManager>
{
public:
	ModelManager();
	~ModelManager();

public:
	bool Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon);
	bool LoadModel(string FileDir, HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon);
	bool AddAnimation(string FileDir, string AnimationFileDir, HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon);
	SkinnedModel* GetModel(string FileDir);

private:
	map<string, SkinnedModel*> ModelMap;
};

