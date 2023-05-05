#pragma once
#include "WModel.h"

class StaticModel : public WModel
{
	StaticModel();
	StaticModel(const StaticModel& model);
	~StaticModel();
};