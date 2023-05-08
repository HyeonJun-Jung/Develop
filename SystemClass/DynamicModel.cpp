#include "DynamicModel.h"

DynamicModel::DynamicModel()
{
}

DynamicModel::DynamicModel(const DynamicModel& mode)
{
}

DynamicModel::~DynamicModel()
{
}

bool DynamicModel::Render()
{
    BaseModel::Render();
    return true;
}

bool DynamicModel::Update(float fDeltaTime) {
    BaseModel::Update(fDeltaTime);
    return true;
}