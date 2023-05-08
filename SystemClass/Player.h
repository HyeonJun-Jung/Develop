#pragma once
#include "DynamicModel.h"
class Player : public DynamicModel
{
public:
	Player();
	~Player();

public:
	bool Render();
	bool Update(float fDeltaTime);

};