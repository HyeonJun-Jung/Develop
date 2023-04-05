#pragma once
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>
#include "Flag.h"
#include "Singleton.h"
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass : public Singleton<CameraClass>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

public:
	XMFLOAT3 GetCameraForwardDirection();
	XMFLOAT3 GetCameraRightDirection();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void CameraMove(Camera_Move_Dir MoveDir);
	void CameraRotation(Camera_Rotation_Dir RotationDir);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetLookAt();

	void Render();
	void GetViewMatrix(XMMATRIX&);

	void ReflectionRender(float);
	void GetReflectionViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;

	XMFLOAT3 m_up;
	XMFLOAT3 m_lookAt;

	XMVECTOR m_UpVector;
	XMVECTOR m_LookAtVector;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_reflectionViewMatrix;

	XMFLOAT3 m_CameraFoward;
	XMFLOAT3 m_CameraRight;

};

#endif
