#pragma once
#include "Player.h"
class InputClass;
class Player;
class PlayerState
{
public:
	virtual ~PlayerState() {};
	virtual PlayerState* UpdateInput(InputClass* input, Player* player, float fDeltaTime) { return nullptr; }
	virtual void Enter(Player* player) { player->SetNextAnimation(m_AnimationIndex);  }

public:
	int m_AnimationIndex = -1;
};

