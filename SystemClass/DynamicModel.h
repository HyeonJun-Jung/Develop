#pragma once
#include "WModel.h"

class DynamicModel : public WModel
{
public:
	DynamicModel();
	DynamicModel(const DynamicModel& mode);
	~DynamicModel();

	bool Render();
	bool Update(float fDeltaTime);

};