#pragma once
#include "..\stdafx.h"
#include "ModelStructure.h"

//
//	�⺻���� �޽���� �ؽ���, �ִϸ��̼� ���� ���� ���� Ŭ����
//	ModelLoader�� ���ؼ� �����Ǿ� ModelManager�� ModelMap���� ����ȴ�.
//	ModelManager�� ���� �ѹ� �ε�� ���� ModelMap���� �ҷ�������.
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
	// ���� ����, �ε��� ����, ���׸��� ����
	vector<MtMesh*> MeshList;

	// �ִϸ��̼� ����
	vector<Animation> AnimationList;

	// Bone ����
	map<string, int> BoneMap;
	vector<boneInfo> BoneInfos;

	// Node ���� ����
	NodeInfo* RootNode = new NodeInfo;

	// Obb �ִ� �ּ�
	XMFLOAT3 MaxPos, MinPos;

public:
	XMMATRIX GlobalInverseTransform = XMMatrixIdentity();

	// ���Ǵ� ���׸��� ����
	MarterialType Mt_Type;
};

