#pragma once
#include "BaseModel.h"

class StaticModel : public BaseModel
{
public:
	StaticModel();
	StaticModel(const StaticModel& model);
	~StaticModel();

public:
	virtual bool Render();

};