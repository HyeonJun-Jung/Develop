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

		// Texture�� Type�� ���� PixelShader�� ���
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

	ReadNodeHierarchy(AnimationIndex, AnimationTime, RootNode, XMMatrixIdentity(), BoneFinalTransform);
}

void SkinnedModel::ReadNodeHierarchy(int AnimationIndex, float AnimationTime, NodeInfo* node, const XMMATRIX& parentTransform, XMMATRIX* BoneFinalTransform)
{
	string nodeName(node->name);
	Animation animation = AnimationList[AnimationIndex];
	XMMATRIX globalTransform = XMMatrixIdentity();


	//////////////////////////////////////////
	// 
	//	�ִϸ��̼��� play���� ������ node->transform�� BoneFinalTransform�� ����� ���� ���� �ʴ´�.
	// 
	// //////////////////////////////////////
	

	XMMATRIX nodeTransform = XMMatrixIdentity();	

	int nodeAnimIndex = FindAniNodeIndex(&animation, nodeName);

	//���� ��ȯ ������ TM����� ����� ���� ���Ǵ� ���� ����
	XMVECTOR Z = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	if (nodeAnimIndex != -1)
	{
		Aninode aninode = animation.aninodeList[nodeAnimIndex];
		if (aninode.KeyFrames.front().timePos >= AnimationTime) {
			XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.front().trans);
			XMVECTOR S = XMLoadFloat3(&aninode.KeyFrames.front().scale);
			XMVECTOR R = XMLoadFloat4(&aninode.KeyFrames.front().rotation);

			nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
		}
		//���� ������ �ð��� ������ Ű ������ �ð����� ���� ���
		else if (aninode.KeyFrames.back().timePos <= AnimationTime) {
			XMVECTOR T = XMLoadFloat3(&aninode.KeyFrames.back().trans);
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

					//�� ������ ���� ���� ����
					XMVECTOR T = XMVectorLerp(t1, t2, lerpRate);
					XMVECTOR S = XMVectorLerp(s1, s2, lerpRate);
					XMVECTOR R = XMQuaternionSlerp(r1, r2, lerpRate);

					nodeTransform = XMMatrixAffineTransformation(S, Z, R, T);
					break;
				}
			}
		}
	}

	globalTransform = nodeTransform * parentTransform;
	// globalTransform = nodeTransform;

	if (BoneMap.find(nodeName) != BoneMap.end())
	{
		unsigned int boneIndex = BoneMap[nodeName];

		// boneOffset�� model space���� ���ǵǾ��� �������� bone space���� ���ǵǵ��� ����� ��
		// XMMatrixTranspose�� ���� hlsl���� �ùٸ��� ����� ����� �� �ֵ��� ��.
		BoneFinalTransform[boneIndex] = XMMatrixTranspose(BoneInfos[boneIndex].boneoffset * globalTransform * GlobalInverseTransform);
	}

	for (int i = 0; i < node->children.size(); i++)
		ReadNodeHierarchy(AnimationIndex, AnimationTime, node->children[i], globalTransform, BoneFinalTransform);
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
	//���� ��ȯ ������ TM����� ����� ���� ���Ǵ� ���� ����
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
		//���� ������ �ð��� ������ Ű ������ �ð����� ���� ���
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

					//�� ������ ���� ���� ����				
					
					XMVECTOR T = XMVectorLerp(t1, t2, lerpRate);
					T = XMVectorSetY(T, 0);
					XMVECTOR S = XMVectorLerp(s1, s2, lerpRate);
					XMVECTOR R = XMQuaternionSlerp(r1, r2, lerpRate);

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
