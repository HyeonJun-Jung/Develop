#pragma once
#include <list>
#include <string>
#include "..\stdafx.h"

class Object;
class InputClass;

class Layer
{
public:
	Layer(std::string layerName, int priority);
	virtual ~Layer();

public:
	virtual bool Initialize(ID3D11Device* dev);
	virtual bool Render(ID3D11DeviceContext* devcon); 
	virtual bool PostRender(ID3D11DeviceContext* devcon);
	virtual bool Update(ID3D11DeviceContext* devcon, float fDeltaTime);
	virtual bool UpdateInput(InputClass* input, float fDeltaTime);
	virtual bool UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime);
	virtual bool UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime);


public:
	void InsertModel(Object* model);
	int GetPriority();
	std::string GetName();

protected:
	std::string m_LayerName;
	int m_Priority;
	std::list<Object*> m_ModelList;
};

