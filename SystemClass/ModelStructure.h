#pragma once
#include "..\stdafx.h"

struct VERTEX {
	XMFLOAT3 position;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
	XMFLOAT4 BoneWeights = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMUINT4 BoneIds = XMUINT4(0, 0, 0, 0);
};

struct WMType {
	float Transx = 0; float Transy = 0; float Transz = 0;
	float RotationX = 0; float RotationY = 0; float RotationZ = 0;
	float ScaleX = 1; float ScaleY = 1; float ScaleZ = 1;
};

struct MarterialType {
	bool Diffuse = false;
	bool Normal = false;
	bool Specular = false;
	bool Glossiness = false;
	bool Emissive = false;
};

struct VectorType {
	float x, y, z;
};

struct boneInfo {
	XMMATRIX boneoffset;
	XMMATRIX finalTransform;
};

struct NodeInfo {
	std::string name;
	XMMATRIX transform;
	std::vector<NodeInfo*> children;
};

struct KeyFrame {
	float		timePos = 0.f;
	XMFLOAT3	trans = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT4	rotation = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT3	scale = XMFLOAT3(0.f, 0.f, 0.f);
};


struct Aninode {
	string					 name = "none";				// ����� ��� �̸�
	vector<KeyFrame>		KeyFrames;						// Ű������ ����Ʈ

	// XMMATRIX				Trans = XMMatrixIdentity();		//	�����Ӹ��� ������Ʈ�Ǵ� ���, Bone Matrix�� �������� �̰��� ����� �� ���
};


struct Animation {
	string name = "none";		// �̸�
	float TicksPerSecond = 0.f;		// 1�ʿ� ��� �������� ����ϴ���
	float Duration = 0.f;				// �ִϸ��̼� �� ��� �ð�
	float PlayTime = 0.f;				// ���� ��� ������
	float LastTime = 0.f;				// ������ ������

	std::vector<Aninode>	aninodeList;	//�ִϸ��̼� ��� ����Ʈ
};


struct Marterial {
	std::string type;
	std::string path;
	ID3D11ShaderResourceView* texture;
};

struct MtMesh {
	string Name = "none";
	vector<Marterial> Textures;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	unsigned int VertexCount = 0;
	unsigned int IndexCount = 0;
};

struct Sphere {
	Sphere(float x, float y, float z)
	{
		centerPos = XMFLOAT3(x, y, z);
	}
	XMFLOAT3 centerPos;
	float radius;
};