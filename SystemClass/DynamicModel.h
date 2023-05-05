#pragma once
#include "WModel.h"

class DynamicModel : public WModel
{
public:
	bool Render(ID3D11DeviceContext* devcon, float fDeltatime);
	bool Update(float fDeltaTime);

};