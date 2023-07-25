#include "Layer.h"
#include "Object.h"
Layer::Layer(std::string layerName, int priority)
{
	m_LayerName = layerName;
	m_Priority = priority;
}

Layer::~Layer()
{
}

bool Layer::Initialize(ID3D11Device* dev)
{
	return true;
}

bool Layer::Render(ID3D11DeviceContext* devcon)
{
	std::list<Object*>::iterator iter;
	std::list<Object*>::iterator iterend = m_ModelList.end();

	for (iter = m_ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->Render(devcon))
			return false;
	}

	return true;
}

bool Layer::PostRender(ID3D11DeviceContext* devcon)
{
	std::list<Object*>::iterator iter;
	std::list<Object*>::iterator iterend = m_ModelList.end();

	for (iter = m_ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->PostRender(devcon))
			return false;
	}

	return true;
}

bool Layer::Update(ID3D11DeviceContext* devcon, float fDeltaTime)
{
	std::list<Object*>::iterator iter;
	std::list<Object*>::iterator iterend = m_ModelList.end();

	for (iter = m_ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->Update(devcon, fDeltaTime))
			return false;
	}
	return true;
}

bool Layer::UpdateInput(InputClass* input, float fDeltaTime)
{
	std::list<Object*>::iterator iter;
	std::list<Object*>::iterator iterend = m_ModelList.end();

	for (iter = m_ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->UpdateInput(input, fDeltaTime))
			return false;
	}
	return true;
}

bool Layer::UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime)
{
	std::list<Object*>::iterator iter;
	std::list<Object*>::iterator iterend = m_ModelList.end();

	for (iter = m_ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->UpdateAnimation(devcon, fDeltaTime))
			return false;
	}
	return true;
}

bool Layer::UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime)
{
	std::list<Object*>::iterator iter;
	std::list<Object*>::iterator iterend = m_ModelList.end();

	for (iter = m_ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->UpdatePhysics(devcon, fDeltaTime))
			return false;
	}
	return true;
}

void Layer::InsertModel(Object* model)
{
	m_ModelList.push_back(model);
}

int Layer::GetPriority()
{
	return m_Priority;
}

std::string Layer::GetName()
{
	return m_LayerName;
}
