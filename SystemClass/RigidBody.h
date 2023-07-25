#pragma once
#include "Collider.h"

class RigidBody
{
public:
	RigidBody();
	~RigidBody();

public:
	void Initialize(XMFLOAT3 minPos, XMFLOAT3 maxPos);
	OBB* GetOBB() { return &m_OBB; }
	AABB* GetAABB() { return &m_AABB; }
	Sphere* GetSphere() { return &m_Sphere; }

	static bool OBBCollision(RigidBody* R1, RigidBody* R2);
	static bool SphereCollision(RigidBody* R1, RigidBody* R2);
	static bool AABBCollision(RigidBody* R1, RigidBody* R2);

private:
	OBB m_OBB;
	AABB m_AABB;
	Sphere m_Sphere;

};

