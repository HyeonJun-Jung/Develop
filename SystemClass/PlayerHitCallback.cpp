#include "PlayerHitCallback.h"
#include "ControllerPhysicsC.h"
#include "Player.h"
PlayerHitCallback::PlayerHitCallback()
{
}

PlayerHitCallback::PlayerHitCallback(Player* player, ControllerPhysicsC* physics)
{
    m_Player = player;
    m_ControllerPhysics = physics;
}

PlayerHitCallback::~PlayerHitCallback()
{
}

void PlayerHitCallback::onShapeHit(const PxControllerShapeHit& hit)
{
    const PxShape* shape = hit.shape;
    PxRigidActor* actor = shape->getActor();

    if (hit.shape->getGeometryType() == PxGeometryType::eBOX)
    {
        if (actor && actor->is<PxRigidDynamic>())
        {
            PxRigidDynamic* dynamicActor = static_cast<PxRigidDynamic*>(actor);

            // Modify the velocity of the dynamic actor
            PxVec3 separationVector = hit.dir;
            separationVector.y = 0;
            separationVector.normalize();  // Normalize the separation vector

            float pushForceMagnitude = 300.0f;  // Adjust the magnitude of the push force as needed
            PxVec3 pushForce = separationVector * pushForceMagnitude;

            dynamicActor->addForce(pushForce, PxForceMode::eIMPULSE);
        }
    }

    
}

void PlayerHitCallback::onControllerHit(const PxControllersHit& hit)
{
}

void PlayerHitCallback::onObstacleHit(const PxControllerObstacleHit& hit)
{
}

PxControllerBehaviorFlags PlayerHitCallback::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
    if (shape.getGeometryType() == PxGeometryType::eHEIGHTFIELD)
    {
        return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
    }

    if (shape.getGeometryType() == PxGeometryType::eBOX)
    {
        return PxControllerBehaviorFlag::eCCT_SLIDE;
    }

    return PxControllerBehaviorFlag::eCCT_USER_DEFINED_RIDE;
}

PxControllerBehaviorFlags PlayerHitCallback::getBehaviorFlags(const PxController& controller)
{
    return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags PlayerHitCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
    return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

PxQueryHitType::Enum PlayerHitCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum PlayerHitCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
    return PxQueryHitType::eBLOCK;
}
