#include "PStandingState.h"
#include "PWalkingState.h"
#include "PJumpingState.h"
#include "InputClass.h"
#include "Player.h"
PStandingState::PStandingState()
{
    m_AnimationIndex = 3;
}

PStandingState::~PStandingState()
{
}

PlayerState* PStandingState::UpdateInput(InputClass* input, Player* player, float fDeltaTime)
{
    if (input->m_curkeyboardState[DIK_W] & 0x80 || input->m_curkeyboardState[DIK_A] & 0x80 ||
        input->m_curkeyboardState[DIK_S] & 0x80 || input->m_curkeyboardState[DIK_D] & 0x80)
    {
        return new PWalkingState();
    }
    else if (input->m_curkeyboardState[DIK_SPACE])
    {
        return new PJumpingState();
    }
    return nullptr;
}

void PStandingState::Update(InputClass* input, Player* player, float fDeltaTime)
{
}
