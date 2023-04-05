#pragma once
#include "..\stdafx.h"

// Color Shader ��� ����
struct Vertex {
	XMFLOAT3 position;
	XMFLOAT4 color;
	Vertex()
	{
		position = XMFLOAT3(0.f, 0.f, 0.f);
		color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	}
	Vertex(XMFLOAT3 pos, XMFLOAT4 cor)
	{
		position = pos;
		color = cor;
	}
};

class Obb
{
public:
	Obb();
	Obb(const Obb& Obb);
	~Obb();

public:
	bool SetObb(XMFLOAT3 min, XMFLOAT3 max);
	void Render();

private:
	void CreateCube();
	void CreateAxisVector();
	bool CreateBuffer();
private:
	XMFLOAT3 MinPos, MaxPos;
	XMFLOAT3 Center;

	// Obb ���� �����ϴ� �� ���� �������Ͱ� ����
	// Obb ���� �����ϴ� �� ���� �������ʹ� �� ���� �������Ͱ� �Ǳ⵵ �Ѵ�.
	XMVECTOR AxisUnitVec[3];
	float AxisHalfLength[3];

	XMMATRIX Transformation = XMMatrixIdentity();

	Vertex Cube[8];
	unsigned int indices[36];

	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
};

