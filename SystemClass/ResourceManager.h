#pragma once
#include "Singleton.h"

class ResourceManager : public Singleton<ResourceManager>
{
public:
	ResourceManager();
	~ResourceManager();

private:
	//void LoadModel(WCHAR)

};

