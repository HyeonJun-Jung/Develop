#pragma once
#include "..\stdafx.h"

// Color Shader 사용 예정
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

	// Obb 축을 형성하는 각 변의 단위벡터과 길이
	// Obb 축을 형성하는 각 변의 단위벡터는 각 면의 법선벡터가 되기도 한다.
	XMVECTOR AxisUnitVec[3];
	float AxisHalfLength[3];

	XMMATRIX Transformation = XMMatrixIdentity();

	Vertex Cube[8];
	unsigned int indices[36];

	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
};

