#pragma once
#include "BaseModel.h"

class DynamicModel : public BaseModel
{
public:
	DynamicModel();
	DynamicModel(const DynamicModel& mode);
	~DynamicModel();

	virtual bool Render();
	virtual bool Update(float fDeltaTime);

};