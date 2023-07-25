#include "PJumpingState.h"
#include "PStandingState.h"
#include "SkinnedModel.h"
#include "CameraClass.h"
#include "Player.h"
#include "InputClass.h"
PJumpingState::PJumpingState()
{
	m_AnimationIndex = 1;
}

PJumpingState::~PJumpingState()
{
}

PlayerState* PJumpingState::UpdateInput(InputClass* input, Player* player, float fDeltaTime)
{
	if (m_Entered == false)
	{
		m_Animation = player->GetSkinnedModel()->GetAnimation(m_AnimationIndex);
		m_HighestTime = m_Animation->aninodeList[0].KeyFrames[47].timePos / m_Animation->TicksPerSecond;
		m_Accelalation = m_JumpVelocity / m_HighestTime;

		Update(input, player, fDeltaTime);
		m_Entered = true;
	}
	else
	{
		if (player->IsOnGround())
		{
			// player->GetPlayTime() = m_Animation->aninodeList[0].KeyFrames[66].timePos;
			return new PStandingState();
		}

		Update(input, player, fDeltaTime);
	}

	/*
	if (input->m_curkeyboardState[DIK_W] & 0x80 || input->m_curkeyboardState[DIK_A] & 0x80 ||
		input->m_curkeyboardState[DIK_S] & 0x80 || input->m_curkeyboardState[DIK_D] & 0x80)
	{
		
	}
	*/

	return nullptr;
}

void PJumpingState::Update(InputClass* input, Player* player, float fDeltaTime)
{
	// 23 ������ �� �ְ� ����
	// ��, 23������ ������ �ð����� m_JumpVelocity ��ŭ �����ؾ� ��

	m_JumpVelocity -= m_Accelalation * fDeltaTime;

	if (input->m_curkeyboardState[DIK_W] & 0x80 || input->m_curkeyboardState[DIK_A] & 0x80 ||
		input->m_curkeyboardState[DIK_S] & 0x80 || input->m_curkeyboardState[DIK_D] & 0x80)
	{
		if (m_Velocity < 30)
			m_Velocity += fDeltaTime * 50;

		XMFLOAT3 camera = CameraClass::GetSingleton()->GetRotation();
		XMFLOAT3 rotation = player->GetRotation();

		XMFLOAT3 targetRot = XMFLOAT3(0, 0, 0);
		if (input->m_curkeyboardState[DIK_W] & 0x80)
		{
			//rotation.y = camera.y + 180.f;
			targetRot = XMFLOAT3(0, camera.y + 180.f, 0);
			if (input->m_curkeyboardState[DIK_A] & 0x80)
				targetRot.y -= 45.f;
			if (input->m_curkeyboardState[DIK_D] & 0x80)
				targetRot.y += 45.f;
		}
		else if (input->m_curkeyboardState[DIK_S] & 0x80)
		{
			//rotation.y = camera.y;
			targetRot = XMFLOAT3(0, camera.y, 0);
			if (input->m_curkeyboardState[DIK_A] & 0x80)
				targetRot.y += 45.f;
			if (input->m_curkeyboardState[DIK_D] & 0x80)
				targetRot.y -= 45.f;
		}
		else if (input->m_curkeyboardState[DIK_A] & 0x80)
		{
			//rotation.y = camera.y + 90.f;
			targetRot = XMFLOAT3(0, camera.y + 90, 0);

		}
		else if (input->m_curkeyboardState[DIK_D] & 0x80)
		{
			//rotation.y = camera.y - 90.f;
			targetRot = XMFLOAT3(0, camera.y - 90, 0);
		}

		if (targetRot.y < 0.0f)
		{
			targetRot.y += 360.0f;
		}

		if (targetRot.y > 360.0f)
		{
			targetRot.y -= 360.0f;
		}


		if (targetRot.y < 180)
		{
			// ī�޶��� ȸ�� < �÷��̾��� ȸ��
			if (targetRot.y < rotation.y && rotation.y < targetRot.y + 180.f)
			{
				rotation.y -= fDeltaTime * 180;
				if (rotation.y <= targetRot.y)
					rotation.y = targetRot.y;
			}
			else
			{
				rotation.y += fDeltaTime * 180;
				if (rotation.y >= targetRot.y && rotation.y < 180.f)
					rotation.y = targetRot.y;
			}
		}
		else if (targetRot.y >= 180)
		{
			// �÷��̾��� ȸ�� < ī�޶��� ȸ��
			if (targetRot.y - 180 < rotation.y && rotation.y < targetRot.y)
			{
				rotation.y += fDeltaTime * 180;
				if (rotation.y >= targetRot.y)
					rotation.y = targetRot.y;
			}
			else
			{
				rotation.y -= fDeltaTime * 180;
				if (rotation.y <= targetRot.y && rotation.y >= 180.f)
					rotation.y = targetRot.y;
			}
		}

		if (rotation.y < 0.0f)
		{
			rotation.y += 360.0f;
		}

		if (rotation.y > 360.0f)
		{
			rotation.y -= 360.0f;
		}


		player->SetRotation(rotation);

		XMFLOAT3 position = player->GetPosition();

		float trans_x = -m_Velocity * sin(rotation.y * 0.0174532925f);
		float trans_z = -m_Velocity * cos(rotation.y * 0.0174532925f);

		position = XMFLOAT3(position.x + trans_x * fDeltaTime, position.y, position.z + trans_z * fDeltaTime);

		player->SetVelocity(XMFLOAT3(trans_x * fDeltaTime, m_JumpVelocity * fDeltaTime, trans_z * fDeltaTime));

		player->SetPhysxVelocity(PxVec3(trans_x, 0, trans_z));
	}
	else
	{
		player->SetVelocity(XMFLOAT3(0, m_JumpVelocity* fDeltaTime, 0));
	}
}
