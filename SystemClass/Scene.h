#pragma once
#include <vector>
#include <string>
#include "..\stdafx.h"

class Layer;
class InputClass;

class Scene
{
public:
	Scene();
	~Scene();

public:
	virtual bool Initialize(ID3D11Device* dev);
	virtual bool Render(ID3D11DeviceContext* devcon);
	virtual bool PostRender(ID3D11DeviceContext* devcon);
	virtual bool Update(ID3D11DeviceContext* devcon, InputClass* input, float fDeltaTime);
	void InsertLayer(Layer* layer);
	Layer* GetLayer(std::string layername);

private:
	static int Compare_Layer(Layer* layer1, Layer* layer2);

protected:
	std::vector<Layer*> m_LayerList;

};

