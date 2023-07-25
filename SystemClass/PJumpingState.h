#pragma once
#include "PlayerState.h"
class PJumpingState :
    public PlayerState
{
public:
    PJumpingState();
    virtual ~PJumpingState();

public:
    virtual PlayerState* UpdateInput(InputClass* input, Player* player, float fDeltaTime);
    virtual void Update(InputClass* input, Player* player, float fDeltaTime);

private:
    float m_Velocity = 0;
    float m_JumpVelocity = 300;
    float m_Accelalation = -100;
    float m_PlayTime = 0;
    float m_HighestTime = 0;
    bool m_Entered = false;
    Animation* m_Animation = nullptr;
};

