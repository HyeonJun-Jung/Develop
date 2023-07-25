#pragma once
#include "PlayerState.h"
class PRunningState :
    public PlayerState
{
public:
    PRunningState();
    virtual ~PRunningState();

public:
    virtual PlayerState* UpdateInput(InputClass* input, Player* player, float fDeltaTime);
    virtual void Update(InputClass* input, Player* player, float fDeltaTime);

private:
    float m_Velocity = 100;
};

