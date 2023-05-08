#pragma once
#include <list>

class Layer;

class Scene
{
public:
	Scene();
	~Scene();

public:
	bool Render();
	bool Update(float fDeltaTime);

private:
	std::list<Layer*> LayerList;

};

