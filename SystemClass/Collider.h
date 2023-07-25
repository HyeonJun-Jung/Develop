#pragma once
#include "..\DxDefine.h"
struct OBB {
	OBB()
	{
	}

	OBB(XMFLOAT3 min, XMFLOAT3 max)
	{
		XMFLOAT3 center = XMFLOAT3((max.x + min.x) / 2, (max.y + min.y) / 2, (max.z + min.z) / 2);
		centerPos = XMLoadFloat3(&center);

		XMFLOAT3 axis;
		axis = XMFLOAT3(1, 0, 0);	axisUnitVec[0] = XMLoadFloat3(&axis);
		axis = XMFLOAT3(0, 1, 0);	axisUnitVec[1] = XMLoadFloat3(&axis);
		axis = XMFLOAT3(0, 0, 1);	axisUnitVec[2] = XMLoadFloat3(&axis);

		axisHalfLength[0] = (max.x - min.x) / 2;
		axisHalfLength[1] = (max.y - min.y) / 2;
		axisHalfLength[2] = (max.z - min.z) / 2;
	}

	XMVECTOR centerPos;
	XMVECTOR axisUnitVec[3];
	float axisHalfLength[3];
};

struct AABB {
	AABB()
	{
	}
	AABB(XMFLOAT3 min, XMFLOAT3 max)
	{
		minPos = min; maxPos = max;
	}

	XMFLOAT3 minPos;
	XMFLOAT3 maxPos;
};

struct Sphere {
	Sphere()
	{
	}
	Sphere(XMFLOAT3 min, XMFLOAT3 max)
	{
		XMFLOAT3 center = XMFLOAT3((max.x + min.x) / 2, (max.y + min.y) / 2, (max.z + min.z) / 2);
		centerPos = XMVECTOR(XMLoadFloat3(&center));
		XMFLOAT3 diamiter = XMFLOAT3(max.x - min.x, max.y - min.y, max.z - min.z);
		XMVECTOR diamiterVec = XMVector3Length(XMVECTOR(XMLoadFloat3(&diamiter)));
		radius = XMVectorGetX(diamiterVec) / 2;
	}

	XMVECTOR centerPos;
	float radius;
};