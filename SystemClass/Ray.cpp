#include "Ray.h"
#include "D3DClass.h"
#include "CameraClass.h"
Ray::Ray()
{
}

Ray::~Ray()
{
}

Ray Ray::GetRay(float mouseX, float mouseY)
{
	Ray ray;

	int screenWidth = D3DClass::GetSingleton()->GetScreenWidth();
	int screenHeight = D3DClass::GetSingleton()->GetScreenHeight();

	XMMATRIX inverseViewMatrix; CameraClass::GetSingleton()->GetViewMatrix(inverseViewMatrix);
	inverseViewMatrix = XMMatrixInverse(nullptr, inverseViewMatrix);
	XMFLOAT3X3 inverseViewMatrix3x3;  XMStoreFloat3x3(&inverseViewMatrix3x3, inverseViewMatrix);

	XMMATRIX projectionMatrix;  D3DClass::GetSingleton()->GetProjectionMatrix(projectionMatrix);
	XMFLOAT3X3 projectionMatrix3x3;  XMStoreFloat3x3(&projectionMatrix3x3, projectionMatrix);



	// Projection Matrix������ ���콺 ��ǥ
	// Viewport to Projection
	float pointX = ((2.0f * (float)mouseX) / (float)screenWidth) - 1.0f;
	float pointY = (((2.0f * (float)mouseY) / (float)screenHeight) - 1.0f) * -1.0f;

	// ViewMatrix������ ���콺 ��ǥ
	// Projection to View
	pointX = pointX / projectionMatrix3x3._11;
	pointY = pointY / projectionMatrix3x3._22;

	// ViewMatrix������ Ray ����
	ray.m_Direction.x = pointX;
	ray.m_Direction.y = pointY;
	ray.m_Direction.z = 1;

	// WorldMatrix������ Ray ����
	// View to world
	XMVECTOR direction = XMLoadFloat3(&ray.m_Direction);
	direction = XMVector3TransformNormal(direction, inverseViewMatrix);
	direction = XMVector3Normalize(direction);
	XMStoreFloat3(&ray.m_Direction, direction);

	// Ray ���� ��ǥ = ī�޶� ��ġ
	ray.m_Origin = CameraClass::GetSingleton()->GetPosition();

	return ray;
}
