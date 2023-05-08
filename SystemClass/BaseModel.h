#pragma once
#include "ModelStructure.h"
class BaseModel
{
public:
	BaseModel();
	~BaseModel();

public:
	virtual bool Render();
	virtual bool Update(float fDeltaTime);

	void SetPosition(float PositionX, float PositionY, float PositionZ);
	void SetScale(float ScaleX, float ScaleY, float ScaleZ);
	void SetRotation(float RotationX, float RotationY, float RotationZ);

	void AddPosition(float PositionX, float PositionY, float PositionZ);
	void AddScale(float ScaleX, float ScaleY, float ScaleZ);
	void AddRotation(float RotationX, float RotationY, float RotationZ);

	XMMATRIX GetWorldMatrix();

	// WorldMatrix ¿ä¼Ò        
	WMType m_WMType;
	Resource_Type m_RT = RT_Model;

};

