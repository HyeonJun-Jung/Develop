#include "ModelManager.h"
#include "AssimpTest/ModelLoader.h"
ModelManager* ModelManager::m_Instance = nullptr;

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
}

// 
bool ModelManager::LoadModel(string FileDir, HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon)
{
	if (ModelMap.find(FileDir) == ModelMap.end())
	{

		ModelLoader modelLoader;
		SkinnedModel* model = modelLoader.Load(hwnd, device, devcon, 0, FileDir);

		if (!model)
			return false;

		ModelMap.insert(pair<string, SkinnedModel*>(FileDir, model));
	}
	return true;
}

SkinnedModel* ModelManager::GetModel(string FileDir)
{
	if (ModelMap.find(FileDir) == ModelMap.end())
	{
		return nullptr;
	}
	else
		return ModelMap[FileDir];
}

