#include "ModelManager.h"
#include "AssimpTest/ModelLoader.h"
ModelManager* ModelManager::m_Instance = nullptr;

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{

}

bool ModelManager::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon)
{

	string Filename = "../SystemClass/data/Standard Walk.fbx";
	if (!LoadModel(Filename, hwnd, device, devcon))
		return false;

	string AnimationFilename = "../SystemClass/data/Jumping.fbx";
	if (!AddAnimation(Filename, AnimationFilename, hwnd, device, devcon))
		return false;

	AnimationFilename = "../SystemClass/data/Running.fbx";
	if (!AddAnimation(Filename, AnimationFilename, hwnd, device, devcon))
		return false;

	AnimationFilename = "../SystemClass/data/Idle.fbx";
	if (!AddAnimation(Filename, AnimationFilename, hwnd, device, devcon))
		return false;

	Filename = "../SystemClass/data/Jumping.fbx";
	if (!LoadModel(Filename, hwnd, device, devcon))
		return false;

	 Filename = "../SystemClass/data/Box/Sci-fi Container Game Free.obj";
	if (!LoadModel(Filename, hwnd, device, devcon))
		return false;

	

	return true;
}

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

bool ModelManager::AddAnimation(string FileDir, string AnimationFileDir, HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon)
{
	if (ModelMap.find(FileDir) == ModelMap.end())
	{
		return false;
	}
	else
	{
		ModelLoader modelLoader;
		if (!modelLoader.AddAnimation(hwnd, device, devcon, ModelMap[FileDir], AnimationFileDir))
			return false;
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

