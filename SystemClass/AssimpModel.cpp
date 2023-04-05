#include "AssimpModel.h"
#include "AssimpTest/ModelLoader.h"

AssimpModel::AssimpModel()
{
}

AssimpModel::~AssimpModel()
{
}


bool AssimpModel::LoadModel(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filename)
{
	ModelLoader _ModelLoader;
	//VertexData = _ModelLoader.Load(hwnd, dev, devcon, 0, filename);
	return true;
}

bool AssimpModel::RenderModel()
{
	return true;
}

bool AssimpModel::RenderImGui()
{
	return true;
}
