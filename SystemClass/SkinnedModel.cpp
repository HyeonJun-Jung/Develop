#include "SkinnedModel.h"

SkinnedModel::SkinnedModel()
{
}

SkinnedModel::SkinnedModel(const SkinnedModel& model)
{
}

SkinnedModel::~SkinnedModel()
{
}

void SkinnedModel::Render(ID3D11DeviceContext* devcon)
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	for (int i = 0; i < MeshList.size(); i++)
	{
		devcon->IASetVertexBuffers(0, 1, &MeshList[i]->VertexBuffer, &stride, &offset);
		devcon->IASetIndexBuffer(MeshList[i]->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		SetMarterials(devcon, i);

		devcon->DrawIndexed(MeshList[i]->IndexCount, 0, 0);
	}
}

void SkinnedModel::SetMarterials(ID3D11DeviceContext* devcon, int MeshIndex)
{
	MtMesh* mesh = MeshList[MeshIndex];
	for (int i = 0; i < mesh->Textures.size(); i++)
	{		
		ID3D11ShaderResourceView* texture = mesh->Textures[i].texture;

		// Texture의 Type에 따라 PixelShader에 등록
		if (mesh->Textures[i].type == "texture_diffuse")
		{
			devcon->PSSetShaderResources(0, 1, &texture);
		}
		else if (mesh->Textures[i].type == "texture_specular")
		{
			devcon->PSSetShaderResources(1, 1, &texture);
		}
		else if (mesh->Textures[i].type == "texture_normal")
		{
			devcon->PSSetShaderResources(2, 1, &texture);
		}
		else if (mesh->Textures[i].type == "texture_Glossiness")
		{
			devcon->PSSetShaderResources(3, 1, &texture);
		}
		else if (mesh->Textures[i].type == "texture_Emissive")
		{
			devcon->PSSetShaderResources(4, 1, &texture);
		}
	}

}

void SkinnedModel::UpdateBoneFinalMatrix(int AnimationIndex, float AnimationTime, XMMATRIX* BoneFinalTransform)
{
	if (!MoveNode)
	{
		processMoveNode();
	}
	ReadNodeHierarchy(AnimationIndex, AnimationTime, RootNode, XMMatrixIdentity(), BoneFinalTransform);
}
void SkinnedModel::ReadNodeHierarchy(int AnimationIndex, float AnimationTime, 
									NodeInfo* node, const XMMATRIX& parentTransform, XMMATRIX* BoneFinalTransform)
{
	string nodeName(node->name);
	Animation animation = AnimationList[AnimationIndex];
	XMMATRIX globalTransform = XMMatrixIdentity();


	//////////////////////////////////////////
	// 
	//	애니메이션을 play중일 때에는 node->transform을 BoneFinalTransform을 계산할 때에 쓰지 않는다.
	// 
	// //////////////////////////////////////
	

	XMMATRIX nodeTransform = XMMatrixIdentity();	

	int nodeAnimIndex = FindAniNodeIndex(&animation, nodeName);

	//아핀 변환 정보로 TM행렬을 만들기 위해 사용되는 참조 변수
	XMVECTOR Z = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	if (nodeAnimIndex != -1)
	{
		if (node == MoveNode)
		{
			Aninode aninode = animation.aninodeList[nodeAnimIndex];
			if (aninode.KeyFrames.front().timePos >= AnimationTime) {
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.front().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.front().rotation);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, Z);
			}
			//현재 프레임 시간이 마지막 키 프레임 시간보다 빠른 경우
			else if (aninode.KeyFrames.back().timePos <= AnimationTime) {
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.back().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.back().rotation);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, Z);
			}
			else {
				for (UINT i = 0; i <= aninode.KeyFrames.size() - 1; i++) {
					auto key = aninode.KeyFrames;
					if (AnimationTime >= key[i].timePos && AnimationTime <= key[i + 1].timePos) {
						float lerpRate = (AnimationTime - key[i].timePos) / (key[i + 1].timePos - key[i].timePos);

						XMVECTOR s1 = XMLoadFloat3(&key[i].scale);
						XMVECTOR s2 = XMLoadFloat3(&key[i + 1].scale);

						XMVECTOR r1 = XMLoadFloat4(&key[i].rotation);
						XMVECTOR r2 = XMLoadFloat4(&key[i + 1].rotation);

						//두 프레임 사이 정보 보간
						XMVECTOR S = XMVectorLerp(s1, s2, lerpRate);
						XMVECTOR R = XMQuaternionSlerp(r1, r2, lerpRate);

						nodeTransform = XMMatrixAffineTransformation(S, Z, R, Z);
						break;
					}
				}
			}
		}
		else
		{
			Aninode aninode = animation.aninodeList[nodeAnimIndex];
			if (aninode.KeyFrames.front().timePos >= AnimationTime) {
				XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.front().trans);
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.front().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.front().rotation);

				// 제자리에서의 재생을 위한 Translation 제거
				// T = XMVectorSetX(T, 0);
				// T = XMVectorSetZ(T, 0);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
			}
			//현재 프레임 시간이 마지막 키 프레임 시간보다 빠른 경우
			else if (aninode.KeyFrames.back().timePos <= AnimationTime) {
				XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.back().trans);
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.back().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.back().rotation);

				// 제자리에서의 재생을 위한 Translation 제거
				// T = XMVectorSetX(T, 0);
				// T = XMVectorSetZ(T, 0);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
			}
			else {
				for (UINT i = 0; i <= aninode.KeyFrames.size() - 1; i++) {
					auto key = aninode.KeyFrames;
					if (AnimationTime >= key[i].timePos && AnimationTime <= key[i + 1].timePos) {
						float lerpRate = (AnimationTime - key[i].timePos) / (key[i + 1].timePos - key[i].timePos);

						XMVECTOR s1 = XMLoadFloat3(&key[i].scale);
						XMVECTOR s2 = XMLoadFloat3(&key[i + 1].scale);

						XMVECTOR t1 = XMLoadFloat3(&key[i].trans);
						XMVECTOR t2 = XMLoadFloat3(&key[i + 1].trans);

						XMVECTOR r1 = XMLoadFloat4(&key[i].rotation);
						XMVECTOR r2 = XMLoadFloat4(&key[i + 1].rotation);

						//두 프레임 사이 정보 보간
						XMVECTOR T = XMVectorLerp(t1, t2, lerpRate);
						XMVECTOR S = XMVectorLerp(s1, s2, lerpRate);
						XMVECTOR R = XMQuaternionSlerp(r1, r2, lerpRate);

						// 제자리에서의 재생을 위한 Translation 제거
						// T = XMVectorSetX(T, 0);
						// T = XMVectorSetZ(T, 0);

						nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
						break;
					}
				}
			}		
		}
	}

	globalTransform = nodeTransform * parentTransform;

	if (BoneMap.find(nodeName) != BoneMap.end())
	{
		unsigned int boneIndex = BoneMap[nodeName];

		// boneOffset은 model space에서 정의되었던 정점들을 bone space에서 정의되도록 만드는 것
		// XMMatrixTranspose를 통해 hlsl에서 올바르게 행렬을 사용할 수 있도록 함.
		BoneFinalTransform[boneIndex] = BoneInfos[boneIndex].boneoffset * globalTransform * GlobalInverseTransform;
	}

	for (int i = 0; i < node->children.size(); i++)
		ReadNodeHierarchy(AnimationIndex, AnimationTime, node->children[i], globalTransform, BoneFinalTransform);
}


void SkinnedModel::ChangeAnimation(int prevAni, int nextAni, float animationTime, float LerpTime, XMMATRIX* BoneFinalTransform)
{
	if (!MoveNode)
	{
		processMoveNode();
	}
	ReadNodeHierarchy(prevAni, nextAni, animationTime, LerpTime, RootNode, XMMatrixIdentity(), BoneFinalTransform);
}


void SkinnedModel::ReadNodeHierarchy(int prevAni, int nextAni, float AnimationTime, float LerpTime, NodeInfo* node, const XMMATRIX& parentTransform, XMMATRIX* BoneFinalTransform)
{
	string nodeName(node->name);

	Animation animation = AnimationList[prevAni];
	Animation nextAnimation = AnimationList[nextAni];

	XMMATRIX globalTransform = XMMatrixIdentity();


	//////////////////////////////////////////
	// 
	//	애니메이션을 play중일 때에는 node->transform을 BoneFinalTransform을 계산할 때에 쓰지 않는다.
	// 
	// //////////////////////////////////////


	XMMATRIX nodeTransform = XMMatrixIdentity();

	int nodeAnimIndex = FindAniNodeIndex(&animation, nodeName);

	//아핀 변환 정보로 TM행렬을 만들기 위해 사용되는 참조 변수
	XMVECTOR Z = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	if (nodeAnimIndex != -1)
	{
		if (node == MoveNode)
		{
			Aninode aninode = animation.aninodeList[nodeAnimIndex];
			Aninode nextAninode = nextAnimation.aninodeList[nodeAnimIndex];

			if (aninode.KeyFrames.front().timePos >= AnimationTime) {
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.front().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.front().rotation);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, Z);
			}
			//현재 프레임 시간이 마지막 키 프레임 시간보다 빠른 경우
			else if (aninode.KeyFrames.back().timePos <= AnimationTime) {
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.back().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.back().rotation);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, Z);
			}
			else {
				auto key = aninode.KeyFrames;
				auto nextKey = nextAninode.KeyFrames;

				XMVECTOR s1, s2, s3, S;
				XMVECTOR t1, t2, t3, T;
				XMVECTOR r1, r2, r3, R;

				for (UINT i = 0; i <= aninode.KeyFrames.size() - 1; i++)
				{
					if (AnimationTime >= key[i].timePos && AnimationTime <= key[i + 1].timePos)
					{
						float lerpRate = (AnimationTime - key[i].timePos) / (key[i + 1].timePos - key[i].timePos);

						s1 = XMLoadFloat3(&key[i].scale);
						s2 = XMLoadFloat3(&key[i+1].scale);

						r1 = XMLoadFloat4(&key[i].rotation);
						r2 = XMLoadFloat4(&key[i+1].rotation);

						//두 프레임 사이 정보 보간
						S = XMVectorLerp(s1, s2, lerpRate);
						R = XMQuaternionSlerp(r1, r2, lerpRate);
						break;
					}
				}

				s3 = XMLoadFloat3(&nextKey[0].scale);
				r3 = XMLoadFloat4(&nextKey[0].rotation);

				//두 프레임 사이 정보 보간
				S = XMVectorLerp(S, s3, LerpTime * (10 / 3));
				R = XMQuaternionSlerp(R, r3, LerpTime * (10 / 3));

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, Z);
						

			}
		}
		else
		{
			Aninode aninode = animation.aninodeList[nodeAnimIndex];
			Aninode nextAninode = nextAnimation.aninodeList[nodeAnimIndex];

			if (aninode.KeyFrames.front().timePos >= AnimationTime) {
				XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.front().trans);
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.front().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.front().rotation);

				// 제자리에서의 재생을 위한 Translation 제거
				// T = XMVectorSetX(T, 0);
				// T = XMVectorSetZ(T, 0);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
			}
			//현재 프레임 시간이 마지막 키 프레임 시간보다 빠른 경우
			else if (aninode.KeyFrames.back().timePos <= AnimationTime) {
				XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.back().trans);
				XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.back().scale);
				XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.back().rotation);

				// 제자리에서의 재생을 위한 Translation 제거
				// T = XMVectorSetX(T, 0);
				// T = XMVectorSetZ(T, 0);

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
			}
			else {
				auto key = aninode.KeyFrames;
				auto nextKey = nextAninode.KeyFrames;

				XMVECTOR s1, s2, s3, S;
				XMVECTOR t1, t2, t3, T;
				XMVECTOR r1, r2, r3, R;

				for (UINT i = 0; i <= aninode.KeyFrames.size() - 1; i++)
				{
					if (AnimationTime >= key[i].timePos && AnimationTime <= key[i + 1].timePos)
					{
						float lerpRate = (AnimationTime - key[i].timePos) / (key[i + 1].timePos - key[i].timePos);

						s1 = XMLoadFloat3(&key[i].scale);
						s2 = XMLoadFloat3(&key[i + 1].scale);

						r1 = XMLoadFloat4(&key[i].rotation);
						r2 = XMLoadFloat4(&key[i + 1].rotation);

						t1 = XMLoadFloat3(&key[i].trans);
						t2 = XMLoadFloat3(&key[i+1].trans);

						//두 프레임 사이 정보 보간
						T = XMVectorLerp(t1, t2, lerpRate);
						S = XMVectorLerp(s1, s2, lerpRate);
						R = XMQuaternionSlerp(r1, r2, lerpRate);
	
						break;
					}
				}

				t3 = XMLoadFloat3(&nextKey[0].trans);
				s3 = XMLoadFloat3(&nextKey[0].scale);
				r3 = XMLoadFloat4(&nextKey[0].rotation);

				//두 프레임 사이 정보 보간
				T = XMVectorLerp(T, t3, LerpTime * (10 / 3));
				S = XMVectorLerp(S, s3, LerpTime * (10 / 3));
				R = XMQuaternionSlerp(R, r3, LerpTime * (10 / 3));

				nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
						
				
				
			}
		}
	}

	globalTransform = nodeTransform * parentTransform;

	if (BoneMap.find(nodeName) != BoneMap.end())
	{
		unsigned int boneIndex = BoneMap[nodeName];

		// boneOffset은 model space에서 정의되었던 정점들을 bone space에서 정의되도록 만드는 것
		// XMMatrixTranspose를 통해 hlsl에서 올바르게 행렬을 사용할 수 있도록 함.
		BoneFinalTransform[boneIndex] = BoneInfos[boneIndex].boneoffset * globalTransform * GlobalInverseTransform;
	}

	for (int i = 0; i < node->children.size(); i++)
		ReadNodeHierarchy(prevAni, nextAni, AnimationTime, LerpTime, node->children[i], globalTransform, BoneFinalTransform);
}


float SkinnedModel::GetFrameTime(int animationIdx, int frameIdx)
{
	return AnimationList[animationIdx].aninodeList[0].KeyFrames[frameIdx].timePos;
}

float SkinnedModel::GetCurFrameTime(int animationIdx, float animationTime)
{
	Animation animation = AnimationList[animationIdx];
	Aninode aninode = animation.aninodeList[0];
	auto key = aninode.KeyFrames;
	for (UINT i = 0; i <= aninode.KeyFrames.size() - 1; i++)
	{
		if (animationTime >= key[i].timePos && animationTime <= key[i + 1].timePos)
			return key[i].timePos;
	}

	return 0;
}

float SkinnedModel::GetNextFrameTime(int animationIdx, float animationTime)
{
	Animation animation = AnimationList[animationIdx];
	Aninode aninode = animation.aninodeList[0];
	auto key = aninode.KeyFrames;

	for (UINT i = 0; i <= aninode.KeyFrames.size() - 1; i++)
	{
		if (animationTime >= key[i].timePos && animationTime <= key[i + 1].timePos)
			return key[i + 1].timePos;
	}

	return 0;
}


XMMATRIX SkinnedModel::GetRootNodeTransform(int AnimationIndex, float AnimationTime, NodeInfo* node)
{
	string nodeName(node->name);
	Animation animation = AnimationList[AnimationIndex];
	XMMATRIX nodeTransform = XMMatrixIdentity();

	queue<NodeInfo*> queue;
	queue.push(RootNode);

	NodeInfo* current = RootNode;
	int nodeAnimIndex = -1;
			
	while (!queue.empty())
	{
		current = queue.front();
		queue.pop();

		string NodeName(current->name);
		nodeAnimIndex = FindAniNodeIndex(&animation, NodeName);

		if (nodeAnimIndex == -1)
		{
			for (int i = 0; i < current->children.size(); i++)
			{
				queue.push(current->children[i]);
			}
		}
		else
		{
			break;
		}
	}
	/*
	XMVECTOR translationVector = current->transform.r[3];
	translationVector = XMVectorNegate(translationVector);
	XMMATRIX inverseTranslationMatrix = XMMatrixTranslationFromVector(translationVector);
	*/
	//아핀 변환 정보로 TM행렬을 만들기 위해 사용되는 참조 변수
	XMVECTOR Z = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	if (nodeAnimIndex == -1)
	{
		return XMMatrixIdentity();
	}
	else
	{
		Aninode aninode = animation.aninodeList[nodeAnimIndex];
		if (aninode.KeyFrames.front().timePos >= AnimationTime) {			
			XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.front().trans);
			T = XMVectorSetY(T, 0);
			XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.front().scale);
			XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.front().rotation);
			
			nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
		}
		//현재 프레임 시간이 마지막 키 프레임 시간보다 빠른 경우
		else if (aninode.KeyFrames.back().timePos <= AnimationTime) {	
			XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.back().trans);
			T = XMVectorSetY(T, 0);
			XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.back().scale);
			XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.back().rotation);

			nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
		}
		else {
			for (UINT i = 0; i < aninode.KeyFrames.size() - 1; i++) {
				auto key = aninode.KeyFrames;
				if (AnimationTime >= key[i].timePos && AnimationTime <= key[i + 1].timePos) {
					float lerpRate = (AnimationTime - key[i].timePos) / (key[i + 1].timePos - key[i].timePos);

					XMVECTOR s1 = XMLoadFloat3(&key[i].scale);
					XMVECTOR s2 = XMLoadFloat3(&key[i + 1].scale);

					XMVECTOR t1 = XMLoadFloat3(&key[i].trans);
					XMVECTOR t2 = XMLoadFloat3(&key[i + 1].trans);

					XMVECTOR r1 = XMLoadFloat4(&key[i].rotation);
					XMVECTOR r2 = XMLoadFloat4(&key[i + 1].rotation);

					//두 프레임 사이 정보 보간				
					
					XMVECTOR T = XMVectorLerp(t1, t2, lerpRate);
					T = XMVectorSetY(T, 0);
					XMVECTOR S = XMVectorLerp(s1, s2, lerpRate);
					//XMVECTOR R = XMQuaternionSlerp(r1, r2, lerpRate);
					XMFLOAT3 temp = XMFLOAT3(0, 0, 0);
					XMVECTOR R = XMLoadFloat3(&temp);
					nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
					break;
				}
			}
		}
	}
	return  nodeTransform;
}

int SkinnedModel::FindAniNodeIndex(const Animation* animation, const string nodeName)
{
	for (int i = 0; i < animation->aninodeList.size(); i++)
	{
		if (animation->aninodeList[i].name == nodeName)
			return i;
	}
	return -1;
}

void SkinnedModel::processMoveNode()
{
	Animation animation = AnimationList[0];

	queue<NodeInfo*> queue;
	queue.push(RootNode);

	NodeInfo* current = RootNode;
	int nodeAnimIndex = -1;

	while (!queue.empty())
	{
		current = queue.front();
		queue.pop();

		string NodeName(current->name);
		nodeAnimIndex = FindAniNodeIndex(&animation, NodeName);

		if (nodeAnimIndex == -1)
		{
			for (int i = 0; i < current->children.size(); i++)
			{
				queue.push(current->children[i]);
			}
		}
		else
		{
			break;
		}
	}

	MoveNode = current;
}

Resource_Type SkinnedModel::GetResourceType()
{
	Resource_Type Rt_Type = RT_Model;

	return Rt_Type;
}
