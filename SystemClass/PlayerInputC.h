#pragma once
#include "InputComponent.h"
class Player;
class InputClass;

class PlayerInputC :
    public InputComponent
{
public:
    PlayerInputC();
    virtual ~PlayerInputC();

public:
    bool Update(Player* player, InputClass* input);

};

