#pragma once
#include "ModelStructure.h"

class Obb;

class BaseModel
{
public:
	BaseModel();
	virtual ~BaseModel();

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

	virtual bool CreateObb();
	virtual bool CreateSphere();

protected:
	// WorldMatrix ¿ä¼Ò
	XMMATRIX m_RotationMatrix = XMMatrixIdentity();
	XMMATRIX m_TranslationMatrix = XMMatrixIdentity();
	XMMATRIX m_ScaleMatrix = XMMatrixIdentity();
	XMMATRIX m_WorldMatrix = XMMatrixIdentity();


	Resource_Type m_RT = RT_Model;
	
	Obb* m_Obb = nullptr;
	Sphere* m_Sphere;
	

};

