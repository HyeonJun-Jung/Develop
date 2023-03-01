#include "FrustumClass.h"

FrustumClass::FrustumClass()
{
}

FrustumClass::FrustumClass(const FrustumClass&)
{
}

FrustumClass::~FrustumClass()
{
}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;

	// 투영 행렬을 XMFLOAT4X4 유형으로 변환합니다.
	XMFLOAT4X4 pMatrix;
	XMStoreFloat4x4(&pMatrix, projectionMatrix);


	// 절두체에서 최소 Z 거리를 계산합니다.
	zMinimum = -pMatrix._43 / pMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);

	pMatrix._33 = r;
	pMatrix._43 = -r * zMinimum;
	projectionMatrix = XMLoadFloat4x4(&pMatrix);

	// 뷰 매트릭스와 업데이트 된 투영 매트릭스에서 절두체 매트릭스를 만듭니다.
	XMMATRIX FinalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// 최종 행렬을 XMFLOAT4X4 유형으로 변환합니다.
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, FinalMatrix);



	// 절두체의 평면 방정식 작성
	// 법선벡터(x, y, z)와 법선과 원점사이 거리(w) 설정

	// Calculate near plane of frustum.
	float x = (float)(matrix._14 + matrix._13);
	float y = (float)(matrix._24 + matrix._23);
	float z = (float)(matrix._34 + matrix._33);
	float w = (float)(matrix._44 + matrix._43);
	m_planes[0] = XMVectorSet(x, y, z, w);
	m_planes[0] = XMPlaneNormalize(m_planes[0]);

	// Calculate far plane of frustum.
	x = (float)(matrix._14 - matrix._13);
	y = (float)(matrix._24 - matrix._23);
	z = (float)(matrix._34 - matrix._33);
	w = (float)(matrix._44 - matrix._43);
	m_planes[1] = XMVectorSet(x, y, z, w);
	m_planes[1] = XMPlaneNormalize(m_planes[1]);

	// Calculate left plane of frustum.
	x = (float)(matrix._14 + matrix._11);
	y = (float)(matrix._24 + matrix._21);
	z = (float)(matrix._34 + matrix._31);
	w = (float)(matrix._44 + matrix._41);
	m_planes[2] = XMVectorSet(x, y, z, w);
	m_planes[2] = XMPlaneNormalize(m_planes[2]);

	// Calculate right plane of frustum.
	x = (float)(matrix._14 - matrix._11);
	y = (float)(matrix._24 - matrix._21);
	z = (float)(matrix._34 - matrix._31);
	w = (float)(matrix._44 - matrix._41);
	m_planes[3] = XMVectorSet(x, y, z, w);
	m_planes[3] = XMPlaneNormalize(m_planes[3]);

	// Calculate top plane of frustum.
	x = (float)(matrix._14 - matrix._12);
	y = (float)(matrix._24 - matrix._22);
	z = (float)(matrix._34 - matrix._32);
	w = (float)(matrix._44 - matrix._42);
	m_planes[4] = XMVectorSet(x, y, z, w);
	m_planes[4] = XMPlaneNormalize(m_planes[4]);

	// Calculate bottom plane of frustum.
	x = (float)(matrix._14 + matrix._12);
	y = (float)(matrix._24 + matrix._22);
	z = (float)(matrix._34 + matrix._32);
	w = (float)(matrix._44 + matrix._42);
	m_planes[5] = XMVectorSet(x, y, z, w);
	m_planes[5] = XMPlaneNormalize(m_planes[5]);

	return;
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)
			return false;
	}

	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	// 정육면체의 한 점이라도 절두체의 안에 있는지 확인합니다. 있을 경우 렌더링.

	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter - radius, zCenter - radius, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter + radius, zCenter - radius, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter - radius, zCenter + radius, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter + radius, zCenter + radius, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter - radius, zCenter - radius, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter + radius, zCenter - radius, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter - radius, zCenter + radius, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter + radius, zCenter + radius, 1.0f))) >= 0.0f)
			continue;

		// 정육면체의 한 점도 절두체 내에 없으면 false 반환
		return false;
	}

	return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		// 구의 반경이 절두체 안에 있는지 확인합니다.
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1.0f))) < -radius)
			return false;
	}

	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	// 직육면체의 한 점이라도 절두체의 안에 있는지 확인합니다. 있을 경우 렌더링.

	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter - ySize, zCenter - zSize, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter + ySize, zCenter - zSize, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter - ySize, zCenter + zSize, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter + ySize, zCenter + zSize, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter - ySize, zCenter - zSize, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter + ySize, zCenter - zSize, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter - ySize, zCenter + zSize, 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter + ySize, zCenter + zSize, 1.0f))) >= 0.0f)
			continue;

		// 직육면체의 한 점도 절두체 내에 없으면 false 반환
		return false;
	}

	return true;
}
