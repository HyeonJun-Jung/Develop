#pragma once
#include <list>

class BaseModel;

class Layer
{
public:
	Layer();
	~Layer();

public:
	bool Render();
	bool Update(float fDeltaTime);

protected:
	std::list<BaseModel*> ModelList;
};

