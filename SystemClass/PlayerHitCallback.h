#pragma once
#include "..\stdafx.h"
#include "ControllerCallBack.h"
class Player;
class ControllerPhysicsC;
class PlayerHitCallback : public ControllerCallBack
{
public:
    PlayerHitCallback();
    PlayerHitCallback(Player* player, ControllerPhysicsC*);
    ~PlayerHitCallback();

public:
    // Implements PxUserControllerHitReport
    virtual void onShapeHit(const PxControllerShapeHit& hit);
    virtual void onControllerHit(const PxControllersHit& hit);
    virtual void onObstacleHit(const PxControllerObstacleHit& hit);

    // Implements PxControllerBehaviorCallback
    virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape& shape, const PxActor& actor);
    virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController& controller);
    virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle& obstacle);

    // Implements PxQueryFilterCallback
    virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags);
    virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit);

private: 
    Player* m_Player = nullptr;
    ControllerPhysicsC* m_ControllerPhysics = nullptr;
};

