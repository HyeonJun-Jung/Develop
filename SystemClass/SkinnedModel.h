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
	Animation* GetAnimation(int index) { return &AnimationList[index]; }
	vector<boneInfo>* GetBoneList() { return &BoneInfos; };
	map<string, int>* GetBoneMap() { return &BoneMap; }
	int GetBoneNum() { return BoneInfos.size(); }
	NodeInfo* GetRootNode() { return RootNode; };
	NodeInfo* GetMoveNode() { return MoveNode; }
	XMFLOAT3* GetMaxPos() { return &MaxPos; }
	XMFLOAT3* GetMinPos() { return &MinPos; }
	Resource_Type GetResourceType();
	PxConvexMeshGeometry& GetConvexMeshGeometry() { return ConvexMeshGeometry;}
	PxBoxGeometry& GetBoxGeometry() { return BoxGeometry; }
	PxCapsuleGeometry& GetCapsuleGeometry() { return CapsuleGeometry; }

	XMMATRIX GetRootNodeTransform(int AnimationIndex, float AnimationTime, NodeInfo* node);
	void UpdateBoneFinalMatrix(int AnimationIndex, float AnimationTime, XMMATRIX* BoneFinalTransform);

	void ChangeAnimation(int prevAni, int nextAni, float animationTime, float LerpTime, XMMATRIX* BoneFinalTransform);
	float GetFrameTime(int animationIdx, int frameIdx);

	float GetCurFrameTime(int animationIdx, float animationTime);
	float GetNextFrameTime(int animationIdx, float animationTime);
	
private:
	void ReadNodeHierarchy(int AnimationIndex, float AnimaitonTime, NodeInfo* node, const XMMATRIX& parentTransform, XMMATRIX* BoneFinalTransform);
	void ReadNodeHierarchy(int prevAni, int nextAni, float AnimaitonTime, float LerpTime, NodeInfo* node, const XMMATRIX& parentTransform, XMMATRIX* BoneFinalTransform);
	int FindAniNodeIndex(const Animation* animation, const string nodeName);
	void processMoveNode();
	

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
	NodeInfo* MoveNode = nullptr;

	// Physx Material
	PxConvexMeshGeometry ConvexMeshGeometry;
	PxBoxGeometry BoxGeometry;
	PxCapsuleGeometry CapsuleGeometry;

	// Obb �ִ� �ּ�
	XMFLOAT3 MaxPos, MinPos;
	
	// Last Frame Postion
	XMFLOAT3 LastFramePosition;


public:
	XMMATRIX GlobalInverseTransform = XMMatrixIdentity();

	// ���Ǵ� ���׸��� ����
	MarterialType Mt_Type;
};

