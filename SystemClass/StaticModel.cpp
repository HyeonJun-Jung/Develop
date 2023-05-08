#include "StaticModel.h"

StaticModel::StaticModel()
{

}

StaticModel::StaticModel(const StaticModel& model)
{

}

StaticModel::~StaticModel()
{

}

bool StaticModel::Render()
{
	BaseModel::Render();
	return true;
}

