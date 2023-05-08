#pragma once
#include "..\stdafx.h"
#include "ModelStructure.h"

//
//	기본적인 메쉬들과 텍스쳐, 애니메이션 정보 등을 담은 클래스
//	ModelLoader를 통해서 생성되어 ModelManager의 ModelMap으로 저장된다.
//	ModelManager를 통해 한번 로드된 모델은 ModelMap에서 불러와진다.
//

class SkinnedModel
{
public:
	SkinnedModel(); 
	SkinnedModel(const SkinnedModel& model);
	~SkinnedModel();

public:
	void Render(ID3D11DeviceContext* devcon);
	void SetMarterials(ID3D11DeviceContext* devcon, int MeshIndex);

	vector<MtMesh*>* GetMeshList() { return &MeshList; }
	vector<Animation>* GetAnimationList() { return &AnimationList; }
	vector<boneInfo>* GetBoneList() { return &BoneInfos; };
	map<string, int>* GetBoneMap() { return &BoneMap; }
	int GetBoneNum() { return BoneInfos.size(); }
	NodeInfo* GetRootNode() { return RootNode; };
	XMFLOAT3* GetMaxPos() { return &MaxPos; }
	XMFLOAT3* GetMinPos() { return &MinPos; }
	Resource_Type GetResourceType();

	XMMATRIX GetRootNodeTransform(int AnimationIndex, float AnimationTime, NodeInfo* node);
	void UpdateBoneFinalMatrix(int AnimationIndex, float AnimationTime, XMMATRIX* BoneFinalTransform);

private:
	void ReadNodeHierarchy(int AnimationIndex, float AnimaitonTime, NodeInfo* node, const XMMATRIX& parentTransform, XMMATRIX* BoneFinalTransform);
	int FindAniNodeIndex(const Animation* animation, const string nodeName);

private:
	// 정점 버퍼, 인덱스 버퍼, 마테리얼 정보
	vector<MtMesh*> MeshList;

	// 애니메이션 정보
	vector<Animation> AnimationList;

	// Bone 정보
	map<string, int> BoneMap;
	vector<boneInfo> BoneInfos;

	// Node 계층 구조
	NodeInfo* RootNode = new NodeInfo;

	// Obb 최대 최소
	XMFLOAT3 MaxPos, MinPos;

public:
	XMMATRIX GlobalInverseTransform = XMMatrixIdentity();

	// 사용되는 마테리얼 정보
	MarterialType Mt_Type;
};

