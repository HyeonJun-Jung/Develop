#pragma once
#include "..\stdafx.h"
#include "SkinnedModel.h"
#define MAX_BONE 100

class SkinnedObject;
class AnimationComponent
{
public:
    AnimationComponent();
    virtual ~AnimationComponent(){};

public:
    bool Initialize(ID3D11Device* dev);
    virtual bool Update(ID3D11DeviceContext* devcon, SkinnedObject* obj, float fDeltaTime);

    void SetCurAnimation(int aniIdx) { m_CurAni = aniIdx; }
    void SetNextAnimation(int aniIdx) { m_NextAni = aniIdx; m_AnimationChanged = true; }
    
private:
    bool UpdateBoneMatrix(ID3D11DeviceContext* devcon, SkinnedObject* obj, float fDeltaTime);
   
private:
    static ID3D11Buffer* m_BoneMatrixBuffer;
    static XMMATRIX* m_BoneMatrixs;

private:
    // 애니메이션이나 프레임 교체 시 사용
    int m_NextAni = -1;
    int m_CurFrameTime = -1; int m_NextFrame = -1;
    float m_LerpTime = 0;

    int m_CurAni = -1;
    float m_PlayTime = 0;

    bool m_AnimationChanged = false;

};

