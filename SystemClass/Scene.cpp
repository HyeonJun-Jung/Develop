#include "Scene.h"
#include "Layer.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::Render()
{
	std::list<Layer*>::iterator iter;
	std::list<Layer*>::iterator iterend = LayerList.end();

	for (iter = LayerList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->Render())
			return false;
	}

	return true;
}

bool Scene::Update(float fDeltaTime)
{
	std::list<Layer*>::iterator iter;
	std::list<Layer*>::iterator iterend = LayerList.end();

	for (iter = LayerList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->Update(fDeltaTime))
			return false;
	}

	return true;
}
