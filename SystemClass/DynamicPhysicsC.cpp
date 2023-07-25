#include "DynamicPhysicsC.h"
#include "Object.h"
DynamicPhysicsC::DynamicPhysicsC()
{
}

DynamicPhysicsC::~DynamicPhysicsC()
{
}

bool DynamicPhysicsC::Initialize(const PxTransform& t, const PxGeometry& geometry, PxMaterial* material, 
								float density, float mass, const PxVec3& velocity)
{
	m_Dynamic = PxCreateDynamic(*m_Physics, t, geometry, *material, density);
	if (!m_Dynamic)
		return false;

	m_Dynamic->setMass(mass);
	m_Dynamic->setAngularDamping(0.5f);
	m_Dynamic->setLinearVelocity(velocity);

	m_Scene->addActor(*m_Dynamic);

	return true;
}

bool DynamicPhysicsC::Update(Object* obj)
{
	PxTransform transform = m_Dynamic->getGlobalPose();
	XMFLOAT3 trans = XMFLOAT3(transform.p.x, transform.p.y, transform.p.z);
	XMFLOAT3 scale = XMFLOAT3(1, 1, 1);
	XMFLOAT3 quat = XMFLOAT3(transform.q.x, transform.q.y, transform.q.z);

	XMVECTOR Translation = XMLoadFloat3(&trans);
	XMVECTOR Scale = XMLoadFloat3(&scale);
	XMVECTOR quaternion = XMLoadFloat3(&quat);
	XMVECTOR Z = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	obj->SetPosition(trans);
	obj->SetQuaternion(quat);

	XMMATRIX& worldMatrix = obj->GetWorldMatrix();

	worldMatrix = XMMatrixAffineTransformation(Scale, Z, quaternion, Translation);

	return true;
}
