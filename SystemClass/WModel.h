#pragma once
#include "..\stdafx.h"
#include "ModelStructure.h"

#define MAX_BONES 100

class SkinnedModel;
class BumpMapShaderClass;
class Obb;

class WModel
{
public:
	WModel();
	~WModel();

	bool LoadWModel(string ModelDir);
	void ReleaseModel();

	virtual bool Render();
	virtual bool Update(float fDeltaTime);

	bool SetShader(BumpMapShaderClass* Shader);

	void SetPosition(float PositionX, float PositionY, float PositionZ);
	void SetScale(float ScaleX, float ScaleY, float ScaleZ);
	void SetRotation(float RotationX, float RotationY, float RotationZ);

	void PlayAnimation(int AnimationIndex);
	void StopAnimation();

	void UpdateAnimation(float Deltatime);
	bool SetBoneTransform();
	

private:
	SkinnedModel* m_Skinnedmodel = nullptr;

	BumpMapShaderClass* m_Shader;

	// WorldMatrix 요소        
	WMType m_WMType;

	// Animation 상태
	int PlayAni = -1;				// play Animation Index
	float PlayTime = 0;				// Animation Play Time

	XMMATRIX BoneFinalTransform[MAX_BONES];
	ID3D11Buffer* m_TransformBuffer;

};

