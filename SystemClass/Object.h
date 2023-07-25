#pragma once
#include "..\stdafx.h"
class InputClass;
class Object
{
public:
	virtual ~Object() {}
	virtual bool Update(ID3D11DeviceContext* devcon, float fDeltaTime) { return true; }
	virtual bool UpdateInput(InputClass* input, float fDeltaTime) { return true; }
	virtual bool UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime) { return true; }
	virtual bool UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime) { return true; }
	virtual bool Render(ID3D11DeviceContext* devcon) = 0;
	virtual bool PostRender(ID3D11DeviceContext* devcon);

	
	XMMATRIX& GetWorldMatrix() { return m_WorldMatrix;}

	XMFLOAT3 GetPosition() { return m_Position; }
	XMFLOAT3 GetScale() { return m_Scale; }
	XMFLOAT3 GetRotation() { return m_Rotation; }
	XMFLOAT3 GetQuaternion() { return m_Quaternion; }
	XMFLOAT3 GetVelocity() { return m_Velocity; }
	void SetRotation(XMFLOAT3 rotation) { m_Rotation = rotation; }
	void SetPosition(XMFLOAT3 position) { m_Position = position; }
	void SetScale(XMFLOAT3 scale) { m_Scale = scale; }
	void SetQuaternion(XMFLOAT3 quaternion) { m_Quaternion = quaternion; }
	void SetVelocity(XMFLOAT3 velocity) { m_Velocity = velocity; }

protected:
	XMMATRIX m_WorldMatrix = XMMatrixIdentity();
	XMFLOAT3 m_Position = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_Scale = XMFLOAT3(1, 1, 1);
	XMFLOAT3 m_Rotation = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_Quaternion = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_Velocity = XMFLOAT3(0, 0, 0);
};

