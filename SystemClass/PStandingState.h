#pragma once
#include "PlayerState.h"
class PStandingState :
    public PlayerState
{
public:
    PStandingState();
    virtual ~PStandingState();

public:
    virtual PlayerState* UpdateInput(InputClass* input, Player* player, float fDeltaTime);
    virtual void Update(InputClass* input, Player* player, float fDeltaTime);

private:
};

