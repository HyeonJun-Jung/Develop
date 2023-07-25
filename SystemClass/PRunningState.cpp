#include "PRunningState.h"
#include "PWalkingState.h"
#include "PJumpingState.h"
#include "PlayerState.h"
#include "InputClass.h"
#include "CameraClass.h"
#include "Player.h"

PRunningState::PRunningState()
{
	m_AnimationIndex = 2;
}

PRunningState::~PRunningState()
{
}

PlayerState* PRunningState::UpdateInput(InputClass* input, Player* player, float fDeltaTime)
{
	if (!(input->m_curkeyboardState[DIK_LSHIFT] & 0x80))
	{
		return new PWalkingState();
	} 
	else if (input->m_curkeyboardState[DIK_SPACE] & 0x80)
	{
		return new PJumpingState();
	}
	else if (input->m_curkeyboardState[DIK_W] & 0x80 || input->m_curkeyboardState[DIK_A] & 0x80 ||
		input->m_curkeyboardState[DIK_S] & 0x80 || input->m_curkeyboardState[DIK_D] & 0x80)
	{

		Update(input, player, fDeltaTime);
	}

	return nullptr;
}

void PRunningState::Update(InputClass* input, Player* player, float fDeltaTime)
{

	if (m_Velocity < 200)
		m_Velocity += fDeltaTime * 100;

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
		// 카메라의 회전 < 플레이어의 회전
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
		// 플레이어의 회전 < 카메라의 회전
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


	player->SetVelocity(XMFLOAT3(trans_x * fDeltaTime, -9.8, trans_z * fDeltaTime));

	player->SetPhysxVelocity(PxVec3(trans_x, 0, trans_z));
}
