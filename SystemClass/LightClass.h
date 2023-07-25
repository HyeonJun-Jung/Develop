
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <directxmath.h>
using namespace DirectX;

class BaseModel;

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetModel(BaseModel* model);
	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularPower(float);
	void SetSpecularColor(float, float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();
	float	 GetSpecularPower();
	XMFLOAT4 GetSpecularColor();

private:
	BaseModel* m_Model = nullptr;
	XMFLOAT4 m_position;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	float	 m_specularPower;
	XMFLOAT4 m_specularColor;
};

#endif