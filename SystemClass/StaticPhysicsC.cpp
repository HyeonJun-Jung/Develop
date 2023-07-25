#include "StaticPhysicsC.h"

StaticPhysicsC::StaticPhysicsC()
{
}

StaticPhysicsC::~StaticPhysicsC()
{
}

bool StaticPhysicsC::Initialize(const PxTransform& t, const PxGeometry& geometry, PxMaterial* material)
{
	// m_Static = PxCreateStatic(*m_Physics, t, geometry, *material);

	PxFilterData filter;
	filter.setToDefault();
	//PxFilterFlag::
	filter.word0 = 1;
	PxShape* shape = m_Physics->createShape(geometry, *material);
	shape->setQueryFilterData(filter);
	shape->setSimulationFilterData(filter);
	PxTransform shapeTransform(PxVec3(0.0f, 0.0f, 0.0f)); // 적절한 위치와 회전으로 설정
	m_Static = m_Physics->createRigidStatic(t);
	m_Static->attachShape(*shape);

	m_Scene->addActor(*m_Static);

	return true;
}

bool StaticPhysicsC::Update(Object* obj)
{
	return true;
}
