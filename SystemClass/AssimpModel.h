#pragma once
#include "..\stdafx.h"
#include "ModelStruct.h"



class AssimpModel
{
public:
	AssimpModel();
	~AssimpModel();



public:
	bool LoadModel(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filename);
	bool RenderModel();
	bool RenderImGui();

private:
	
};

