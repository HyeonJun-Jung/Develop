#pragma once
#include "..\stdafx.h"
#include "Shader.h"
#include "DynamicModel.h"

#define MAX_BONES 100

class SkinnedModel;
class BumpMapShaderClass;
class Obb;

class WModel : BaseModel
{
public:
	WModel();
	~WModel();

	bool Initialize(string ModelDir);
	
	virtual bool Render();
	virtual bool Update(float fDeltaTime);

	void PlayAnimation(int AnimationIndex);
	void StopAnimation();

	void UpdateAnimation(float Deltatime);
	bool SetBoneTransform();

private:
	bool LoadWModel(string ModelDir);
	void ReleaseModel();
	

private:
	SkinnedModel* m_Skinnedmodel = nullptr;

	Shader* m_Shader = nullptr;

	// Animation ป๓ลย
	int PlayAni = -1;				// play Animation Index
	float PlayTime = 0;				// Animation Play Time

	XMMATRIX BoneFinalTransform[MAX_BONES];
	ID3D11Buffer* m_TransformBuffer;

};

