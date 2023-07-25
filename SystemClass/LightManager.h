#pragma once
#include "Singleton.h"
#include "..\DxDefine.h"
#include <vector>

class LightClass;
class BaseModel;

class LightManager : public Singleton<LightManager>
{
public:
	LightManager();
	~LightManager();

public:
	bool Initialize(ID3D11Device* dev);
	bool Update(ID3D11DeviceContext* devcon);

	bool RenderLights(ID3D11DeviceContext* devcon);
	bool RenderDirectionalLight(ID3D11DeviceContext* devcon);
	bool RenderPointLights(ID3D11DeviceContext* devcon);

	void SetDirectionalLightModel(BaseModel * model);
	void AddPointLight(LightClass* pointLight);

private:
	ID3D11Buffer* m_DirectionalLightBuffer;
	LightClass* m_DirectionalLight;
	vector<LightClass*> m_PointLightList;
};

