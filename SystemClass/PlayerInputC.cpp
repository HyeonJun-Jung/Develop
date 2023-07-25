#include "PlayerInputC.h"
#include "InputClass.h"
#include "Player.h"
#include "CameraClass.h"
PlayerInputC::PlayerInputC()
{
}

PlayerInputC::~PlayerInputC()
{
}

bool PlayerInputC::Update(Player* player, InputClass* input)
{
	unsigned char keyBoardState[256]; 

	HRESULT result = input->GetKeyBoard()->GetDeviceState(sizeof(keyBoardState), (LPVOID)&keyBoardState);
	if (FAILED(result))
		return false;

	XMFLOAT3 rotation = XMFLOAT3(0, 0, 0);
	XMFLOAT3 CameraRotation = CameraClass::GetSingleton()->GetRotation();

	if (keyBoardState[DIK_W] & 0x80)
	{
		rotation.y += CameraRotation.y;
	}
	else if (keyBoardState[DIK_S] & 0x80)
	{
		rotation.y -= CameraRotation.y;
	}
	if (keyBoardState[DIK_A] & 0x80)
	{
		rotation.y -= 90;
		if (rotation.y < 0.f)
			rotation.y += 360;
	}
	else if (keyBoardState[DIK_D] & 0x80)
	{
		rotation.y += 90;
		if (rotation.y > 360)
			rotation.y -= 360;
	}

	player->SetRotation( rotation);

	return true;
}
