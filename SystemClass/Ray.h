#pragma once
#include "..\stdafx.h"
class Ray
{
public:
	Ray();
	~Ray();

public:
	static Ray GetRay(float mouseX, float mouseY);

public:
	XMFLOAT3 m_Origin;
	XMFLOAT3 m_Direction;
 };

