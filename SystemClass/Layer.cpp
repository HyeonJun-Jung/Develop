#include "Layer.h"
#include "BaseModel.h"
Layer::Layer()
{
}

Layer::~Layer()
{
}

bool Layer::Render()
{
	std::list<BaseModel*>::iterator iter;
	std::list<BaseModel*>::iterator iterend = ModelList.end();

	for (iter = ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->Render())
			return false;
	}

	return true;
}

bool Layer::Update(float fDeltaTime)
{
	std::list<BaseModel*>::iterator iter;
	std::list<BaseModel*>::iterator iterend = ModelList.end();

	for (iter = ModelList.begin(); iter != iterend; iter++)
	{
		if (!(*iter)->Update(fDeltaTime))
			return false;
	}

	return true;
}
