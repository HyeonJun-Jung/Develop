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
	bool LoadModel(string FileDir, HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon);
	SkinnedModel* GetModel(string FileDir);

private:
	map<string, SkinnedModel*> ModelMap;
};

