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

	bool Render(ID3D11DeviceContext* devcon, float fDeltatime);

	bool SetShader(BumpMapShaderClass* Shader);

	void PlayAnimation(int AnimationIndex);
	void StopAnimation();

	void UpdateAnimation(float Deltatime);
	bool SetBoneTransform();
	

private:
	SkinnedModel* m_Skinnedmodel = nullptr;

	BumpMapShaderClass* m_Shader = nullptr;

	// WorldMatrix 요소        
	WMType m_WMType;

	// Animation 상태
	int PlayAni = -1;				// play Animation Index
	float PlayTime = 0;				// Animation Play Time

	XMMATRIX BoneFinalTransform[MAX_BONES];
	ID3D11Buffer* m_TransformBuffer;

};

