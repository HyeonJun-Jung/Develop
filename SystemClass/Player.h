#pragma once
#include "SkinnedObject.h"

class SkinnedGraphicC;
class AnimationComponent;
class SkinnedModel;;
class AnimatedPhysicsC;
class InputClass;
class PlayerState;

class Player : public SkinnedObject
{
public:
	Player();
	~Player();

public:
    virtual bool Initialize(string fileName, SkinnedGraphicC* graphicC, AnimationComponent* animationC);
	virtual bool UpdateInput(InputClass* input, float fDeltaTime);
	virtual bool Render(ID3D11DeviceContext* devcon);
private:
	bool InitializePhysics();

public:
	PlayerState* m_State = nullptr;
};