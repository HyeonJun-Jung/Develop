#include "RigidBody.h"

RigidBody::RigidBody()
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::Initialize(XMFLOAT3 minPos, XMFLOAT3 maxPos)
{
	m_AABB = AABB(minPos, maxPos);
	m_OBB = OBB(minPos, maxPos);
	m_Sphere = Sphere(minPos, maxPos);
}


bool RigidBody::SphereCollision(RigidBody* R1, RigidBody* R2)
{
    Sphere* Sphere1 = R1->GetSphere();
    Sphere* Sphere2 = R2->GetSphere();
    XMVECTOR diffVec = Sphere1->centerPos - Sphere2->centerPos;
    diffVec = XMVector3Length(diffVec);
    float diff = XMVectorGetX(diffVec);

    if (diff <= Sphere1->radius + Sphere2->radius)
        return true;

    return false;
}

bool RigidBody::AABBCollision(RigidBody* R1, RigidBody* R2)
{
    AABB* Box1 = R1->GetAABB();
    AABB* Box2 = R2->GetAABB();

    if (Box1->maxPos.x < Box2->minPos.x || Box2->maxPos.x < Box1->minPos.x)
        return false;

    if (Box1->maxPos.y < Box2->minPos.y || Box2->maxPos.y < Box1->minPos.y)
        return false;

    if (Box1->maxPos.z < Box2->minPos.z || Box2->maxPos.z < Box1->minPos.z)
        return false;

    return true;
}

bool RigidBody::OBBCollision(RigidBody* R1, RigidBody* R2)
{
	OBB* Box1 = R1->GetOBB();
	OBB* Box2 = R2->GetOBB();

    double c[3][3];
    double absC[3][3];
    double d[3];

    double r0, r1, r;
    int i;

    const double cutoff = 0.999999;
    bool existsParallelPair = false;

    XMVECTOR diff = Box1->centerPos - Box2->centerPos;



    for (i = 0; i < 3; ++i)
    {
        c[0][i] = XMVectorGetX(XMVector3Dot(Box1->axisUnitVec[0], Box2->axisUnitVec[i]));
        absC[0][i] = abs(c[0][i]);
        if (absC[0][i] > cutoff)
            existsParallelPair = true;
    }
    d[0] = XMVectorGetX(XMVector3Dot(diff, Box1->axisUnitVec[0]));
    r = abs(d[0]);
    r0 = Box1->axisHalfLength[0];
    r1 = Box2->axisHalfLength[0] * absC[0][0] + Box2->axisHalfLength[1] * absC[0][1] + Box2->axisHalfLength[2] * absC[0][2];

    if (r > r0 + r1)
        return false;



    for (i = 0; i < 3; ++i)
    {
        c[1][i] = XMVectorGetX(XMVector3Dot(Box1->axisUnitVec[1], Box2->axisUnitVec[i]));
        absC[1][i] = abs(c[1][i]);
        if (absC[1][i] > cutoff)
            existsParallelPair = true;
    }
    d[1] = XMVectorGetX(XMVector3Dot(diff, Box1->axisUnitVec[1]));
    r = abs(d[1]);
    r0 = Box1->axisHalfLength[1];
    r1 = Box2->axisHalfLength[0] * absC[1][0] + Box2->axisHalfLength[1] * absC[1][1] + Box2->axisHalfLength[2] * absC[1][2];

    if (r > r0 + r1)
        return false;



    for (i = 0; i < 3; ++i)
    {
        c[2][i] = XMVectorGetX(XMVector3Dot(Box1->axisUnitVec[2], Box2->axisUnitVec[i]));
        absC[2][i] = abs(c[2][i]);
        if (absC[2][i] > cutoff)
            existsParallelPair = true;
    }
    d[2] = XMVectorGetX(XMVector3Dot(diff, Box1->axisUnitVec[2]));
    r = abs(d[2]);
    r0 = Box1->axisHalfLength[2];
    r1 = Box2->axisHalfLength[0] * absC[2][0] + Box2->axisHalfLength[1] * absC[2][1] + Box2->axisHalfLength[2] * absC[2][2];

    if (r > r0 + r1)
        return false;



    r = abs(XMVectorGetX(XMVector3Dot(diff, Box2->axisUnitVec[0])));
    r0 = Box1->axisHalfLength[0] * absC[0][0] + Box1->axisHalfLength[1] * absC[1][0] + Box1->axisHalfLength[2] * absC[2][0];
    r1 = Box2->axisHalfLength[0];

    if (r > r0 + r1)
        return false;



    r = abs(XMVectorGetX(XMVector3Dot(diff, Box2->axisUnitVec[1])));
    r0 = Box1->axisHalfLength[0] * absC[0][1] + Box1->axisHalfLength[1] * absC[1][1] + Box1->axisHalfLength[2] * absC[2][1];
    r1 = Box2->axisHalfLength[1];

    if (r > r0 + r1)
        return false;



    r = abs(XMVectorGetX(XMVector3Dot(diff, Box2->axisUnitVec[2])));
    r0 = Box1->axisHalfLength[0] * absC[0][2] + Box1->axisHalfLength[1] * absC[1][2] + Box1->axisHalfLength[2] * absC[2][2];
    r1 = Box2->axisHalfLength[2];

    if (r > r0 + r1)
        return false;



    if (existsParallelPair == true)
        return true;



    r = abs(d[2] * c[1][0] - d[1] * c[2][0]);
    r0 = Box1->axisHalfLength[1] * absC[2][0] + Box1->axisHalfLength[2] * absC[1][0];
    r1 = Box2->axisHalfLength[1] * absC[0][2] + Box2->axisHalfLength[2] * absC[0][1];
    if (r > r0 + r1)
        return false;



    r = abs(d[2] * c[1][1] - d[1] * c[2][1]);
    r0 = Box1->axisHalfLength[1] * absC[2][1] + Box1->axisHalfLength[2] * absC[1][1];
    r1 = Box2->axisHalfLength[0] * absC[0][2] + Box2->axisHalfLength[2] * absC[0][0];
    if (r > r0 + r1)
        return false;



    r = abs(d[2] * c[1][2] - d[1] * c[2][2]);
    r0 = Box1->axisHalfLength[1] * absC[2][2] + Box1->axisHalfLength[2] * absC[1][2];
    r1 = Box2->axisHalfLength[0] * absC[0][1] + Box2->axisHalfLength[1] * absC[0][0];
    if (r > r0 + r1)
        return false;



    r = abs(d[0] * c[2][0] - d[2] * c[0][0]);
    r0 = Box1->axisHalfLength[0] * absC[2][0] + Box1->axisHalfLength[2] * absC[0][0];
    r1 = Box2->axisHalfLength[1] * absC[1][2] + Box2->axisHalfLength[2] * absC[1][1];
    if (r > r0 + r1)
        return false;



    r = abs(d[0] * c[2][1] - d[2] * c[0][1]);
    r0 = Box1->axisHalfLength[0] * absC[2][1] + Box1->axisHalfLength[2] * absC[0][1];
    r1 = Box2->axisHalfLength[0] * absC[1][2] + Box2->axisHalfLength[2] * absC[1][0];
    if (r > r0 + r1)
        return false;



    r = abs(d[0] * c[2][2] - d[2] * c[0][2]);
    r0 = Box1->axisHalfLength[0] * absC[2][2] + Box1->axisHalfLength[2] * absC[0][2];
    r1 = Box2->axisHalfLength[0] * absC[1][1] + Box2->axisHalfLength[1] * absC[1][0];
    if (r > r0 + r1)
        return false;



    r = abs(d[1] * c[0][0] - d[0] * c[1][0]);
    r0 = Box1->axisHalfLength[0] * absC[1][0] + Box1->axisHalfLength[1] * absC[0][0];
    r1 = Box2->axisHalfLength[1] * absC[2][2] + Box2->axisHalfLength[2] * absC[2][1];
    if (r > r0 + r1)
        return false;



    r = abs(d[1] * c[0][1] - d[0] * c[1][1]);
    r0 = Box1->axisHalfLength[0] * absC[1][1] + Box1->axisHalfLength[1] * absC[0][1];
    r1 = Box2->axisHalfLength[0] * absC[2][2] + Box2->axisHalfLength[2] * absC[2][0];
    if (r > r0 + r1)
        return false;



    r = abs(d[1] * c[0][2] - d[0] * c[1][2]);
    r0 = Box1->axisHalfLength[0] * absC[1][2] + Box1->axisHalfLength[1] * absC[0][2];
    r1 = Box2->axisHalfLength[0] * absC[2][1] + Box2->axisHalfLength[1] * absC[2][0];
    if (r > r0 + r1)
        return false;



    return true;
}
