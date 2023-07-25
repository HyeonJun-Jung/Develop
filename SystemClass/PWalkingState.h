#pragma once
#include "PlayerState.h"
class InputClass;
class Player;
class PWalkingState :
    public PlayerState
{
public:
    PWalkingState();
    virtual ~PWalkingState();

public:
    virtual PlayerState* UpdateInput(InputClass* input, Player* player, float fDeltaTime);
    virtual void Update(InputClass* input, Player* player, float fDeltaTime);

private:
    float m_Velocity = 0;
};

