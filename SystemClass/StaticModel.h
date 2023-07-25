#pragma once
#include "BaseModel.h"

class StaticModel : public BaseModel
{
public:
	StaticModel();
	StaticModel(const StaticModel& model);
	virtual ~StaticModel();

public:
	virtual bool Render();

};