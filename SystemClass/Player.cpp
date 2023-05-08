#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Render()
{
    DynamicModel::Render();
    return true;
}

bool Player::Update(float fDeltaTime)
{
    DynamicModel::Update(fDeltaTime);
    return true;
}
